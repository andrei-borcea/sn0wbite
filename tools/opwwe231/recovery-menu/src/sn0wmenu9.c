/* sn0wmenu: boot menu for the OnePlus Watch 2 recovery (static ARM32, no libdrm).
 *
 * Runs as the init service "recovery"; the stock /system/bin/recovery stays in the image and is started from the menu.
 *   sn0wmenu            the menu: WEAR OS, DUAL BOOT (targets from sn0w-targets.conf), RECOVERY STOCK, FASTBOOT, INFO, SPEGNI
 *   sn0wmenu --deadman  watchdog, a second init service: when it exits, init reboots the watch
 *
 * Fail-open rules (the recovery has no usable adb shell, so these are what keeps the watch from getting stuck):
 *  - the BCB in misc is zeroed first thing and before every exit, so a crash or a hardware reset boots Wear OS. It is set only
 *    while a dual boot payload runs: a payload that resets the watch lands back in this menu once, and the next start clears it
 *  - leaving always goes through sys.powerctl (the recovery SELinux domain has no sys_boot, a raw reboot() is denied)
 *  - the menu leaves by itself after idle_s without a key; the deadman covers a hung or dead menu
 *  - flight recorder in misc at 0x9000, read it from Android with read_flightlog.sh
 * Display: the first SETCRTC fails with EINVAL while the bootloader splash is up. Closing the device (kernel lastclose) disables
 * the pipeline and the immediate reopen works (observed on this panel, notes/flightlog_v8_probe_2026-10-24.txt).
 * Keys: POWER = select, any other key = next. The recovery has no touch device.
 * The verbose window under the menu shows the last log lines: everything flog() writes to the flight recorder, plus vlog() lines
 * (every key press, the output of a dual boot payload) that stay on screen only.
 *
 * Test options (Android shell, nothing is flashed): --sim never touches the real misc, display, inputs, reboot or the stock
 * recovery; --misc FILE gives it a scratch misc, --keys "np.w" scripts the keys (n next, p select, . 250 ms, w 1 s),
 * --real-inputs also reads the key devices, --keylog only logs their key presses (to learn which button is which), --dump DIR writes every distinct frame as PPM, --idle/--slack/--stuck/--grace/--cap/--batt-min/--payload-max change the timers,
 * --hang N freezes the menu after N seconds (deadman test), --targets FILE, --wd FILE, --batt-dir DIR (capacity + status files).
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/system_properties.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <drm/drm.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_mode.h>
#include "ui9.h"

#define BUILD "V9B"
#define STOCK "/system/bin/recovery"
#define CONF "/system/etc/sn0w-targets.conf"
#define WD_FILE "/tmp/sn0w_wd"
#define STATE_OFF 0x8000 /* "SN0W" + label: a dual boot attempt is in flight */
#define LOG_OFF 0x9000
#define LOG_MAX 24000
#define MAXT 8
#define MAXIN 8
#define LBITS (sizeof(long) * 8)

enum { A_WEAROS, A_DUAL, A_STOCK, A_FASTBOOT, A_INFO, A_OFF, A_BACK, A_TARGET };
enum { S_MAIN, S_DUAL, S_INFO };

static int sim, real_inputs, keylog, hang_s, nin, disp_tries;
static int idle_s = 120, slack_s = 45, stuck_s = 20, grace_s = 60, cap_s = 3000, batt_min = 15, payload_max_s = 600;
static const char *misc_path, *conf_path = CONF, *wd_path = WD_FILE, *dump_dir, *keys, *batt_dir = "/sys/class/power_supply/battery";
static long t0, last_input, notice_until;
static int wd_fd = -1;
static char notice_buf[32], bcb_in[24], failed_label[28];
static int dirty = 1;

/* ---------------- verbose window: ring of the last lines, upper case, hard-wrapped to the window width ---------------- */
#define RING_N 48
static char ring[RING_N][LOG_COLS + 1];
static int ring_n; /* lines ever added; the newest is ring[(ring_n - 1) % RING_N] */

static void ring_add(const char *s) {
    int first = 1;
    while (*s || first) {
        char *d = ring[ring_n++ % RING_N];
        int n = 0;
        if (!first) { d[n++] = ' '; d[n++] = ' '; }
        for (; *s && n < LOG_COLS; s++) d[n++] = *s >= 'a' && *s <= 'z' ? *s - 32 : (*s >= ' ' && *s < 127) ? *s : '?';
        d[n] = 0;
        first = 0;
    }
    dirty = 1;
}

static int ring_view(const char **p, int max) { /* the last max lines, oldest first */
    int n = ring_n < max ? ring_n : max;
    for (int i = 0; i < n; i++) p[i] = ring[(ring_n - n + i) % RING_N];
    return n;
}

static long mono_ms(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return t.tv_sec * 1000L + t.tv_nsec / 1000000L; }
static long mono(void) { return mono_ms() / 1000; }

/* ---------------- misc: BCB, in-flight mark, flight recorder ---------------- */
static int misc_fd(void) {
    static const char *c[] = {"/dev/block/bootdevice/by-name/misc", "/dev/block/by-name/misc", 0};
    if (misc_path) return open(misc_path, O_RDWR | O_CREAT, 0600);
    if (sim) return -1;
    for (int i = 0; c[i]; i++) { int fd = open(c[i], O_RDWR); if (fd >= 0) return fd; }
    return -1;
}

static int misc_read(off_t off, void *b, size_t n) {
    int fd = misc_fd();
    if (fd < 0) return -1;
    ssize_t r = pread(fd, b, n, off);
    close(fd);
    return r == (ssize_t)n ? 0 : -1;
}

static void misc_write(off_t off, const void *b, size_t n) {
    int fd = misc_fd();
    if (fd < 0) return;
    pwrite(fd, b, n, off);
    fsync(fd);
    close(fd);
}

static void emit(int persist, const char *fmt, va_list ap) {
    char msg[200], m[240], r[240];
    vsnprintf(msg, sizeof msg, fmt, ap);
    long sec = mono() - t0;
    int n = snprintf(m, sizeof m, "[%ld] %s\n", sec, msg);
    if (n >= (int)sizeof m) { n = sizeof m - 1; m[n - 1] = '\n'; }
    fwrite(m, 1, n, stderr);
    snprintf(r, sizeof r, "%ld %s", sec, msg);
    ring_add(r);
    if (!persist) return;
    int fd = misc_fd();
    if (fd < 0) return;
    flock(fd, LOCK_EX);
    struct { char magic[4]; uint32_t len; } h;
    if (pread(fd, &h, 8, LOG_OFF) != 8 || memcmp(h.magic, "SN0L", 4) || h.len > LOG_MAX) { memcpy(h.magic, "SN0L", 4); h.len = 0; }
    if (h.len + n > LOG_MAX) {
        static const char w[] = "[log wrapped]\n";
        pwrite(fd, w, sizeof w - 1, LOG_OFF + 8);
        h.len = sizeof w - 1;
    }
    pwrite(fd, m, n, LOG_OFF + 8 + h.len);
    h.len += n;
    pwrite(fd, &h, 8, LOG_OFF);
    fsync(fd);
    flock(fd, LOCK_UN);
    close(fd);
}

static void flog(const char *fmt, ...) { va_list ap; va_start(ap, fmt); emit(1, fmt, ap); va_end(ap); } /* flight recorder + window */
static void vlog(const char *fmt, ...) { va_list ap; va_start(ap, fmt); emit(0, fmt, ap); va_end(ap); } /* window only */

/* BCB: command[32] status[32] recovery[768] ...; the bootloader acts on "boot-recovery" at every boot until it is zeroed */
static void bcb_write(int recovery) {
    char z[2048] = {0};
    if (recovery) { strcpy(z, "boot-recovery"); strcpy(z + 64, "recovery\n"); }
    misc_write(0, z, sizeof z);
}

struct mark { char magic[4], label[28]; };

static void mark_set(const char *label) {
    struct mark m;
    memset(&m, 0, sizeof m);
    if (label) { memcpy(m.magic, "SN0W", 4); snprintf(m.label, sizeof m.label, "%s", label); }
    misc_write(STATE_OFF, &m, sizeof m);
}

static int mark_get(char *label, size_t n) {
    struct mark m;
    if (misc_read(STATE_OFF, &m, sizeof m) || memcmp(m.magic, "SN0W", 4) || m.label[0] < ' ') return 0;
    snprintf(label, n, "%.27s", m.label);
    return 1;
}

/* ---------------- environment ---------------- */
static int rd(const char *p, char *buf, int n) {
    int fd = open(p, O_RDONLY);
    if (fd < 0) return -1;
    int r = read(fd, buf, n - 1);
    close(fd);
    if (r < 0) return -1;
    buf[r] = 0;
    while (r > 0 && (buf[r - 1] == '\n' || buf[r - 1] == ' ')) buf[--r] = 0;
    return r;
}

static int batt(int *charging) {
    char b[32], s[32] = {0}, p[96];
    snprintf(p, sizeof p, "%s/capacity", batt_dir);
    if (rd(p, b, sizeof b) < 0) return -1;
    snprintf(p, sizeof p, "%s/status", batt_dir);
    rd(p, s, sizeof s);
    *charging = (s[0] == 'C' || s[0] == 'F');
    return atoi(b);
}

/* ---------------- watchdog: heartbeat file written by the menu, read by the init-supervised deadman ---------------- */
static void beat(int pinned) {
    static int err_logged;
    if (wd_fd < 0) wd_fd = open(wd_path, O_WRONLY | O_CREAT | O_CLOEXEC, 0644);
    if (wd_fd < 0) { if (!err_logged++) flog("beat: cannot open %s errno=%d", wd_path, errno); return; }
    char b[64];
    int n = snprintf(b, sizeof b, "%012ld %012ld %d\n", mono(), last_input + idle_s + slack_s, pinned);
    pwrite(wd_fd, b, n, 0);
}

/* The deadman stands down for a pinned session (the stock recovery, which clears the BCB itself and reboots on its own). Two bad
 * readings in a row are needed, so a torn read of the heartbeat file never reboots the watch. When this returns, init reboots. */
static int deadman_main(void) {
    long start = mono();
    int bad = 0;
    flog("deadman armed: idle %d+%ds, stuck %ds, grace %ds, cap %ds, battery %d%%", idle_s, slack_s, stuck_s, grace_s, cap_s, batt_min);
    for (;; sleep(1)) {
        char b[64] = {0};
        long hb = 0, dl = 0, now = mono();
        int pin = 0, ch = 1, cap;
        const char *why = 0;
        int have = rd(wd_path, b, sizeof b) > 0 && sscanf(b, "%ld %ld %d", &hb, &dl, &pin) == 3;
        if (have && pin) { bad = 0; continue; }
        if (!have) { if (now - start > grace_s) why = "no menu heartbeat"; }
        else if (now - hb > stuck_s) why = "menu stuck or gone";
        else if (now > dl) why = "idle deadline";
        if (!why && now - start > cap_s) why = "session cap";
        if (!why && (cap = batt(&ch)) >= 0 && cap <= batt_min && !ch) why = "low battery";
        bad = why ? bad + 1 : 0;
        if (bad >= 2) {
            flog("DEADMAN: %s -> clearing the BCB, init reboots", why);
            bcb_write(0);
            return 0;
        }
    }
}

/* ---------------- display: stock minui sequence (legacy DRM calls only), see the header comment ---------------- */
#define FOURCC_XBGR8888 0x34324258u /* ro.minui.pixel_format=RGBX_8888 in the stock recovery */
struct fbuf { uint32_t handle, pitch, fb_id; uint64_t size; uint32_t *map; };
static int drm_fd = -1, cur, flip_logged;
static uint32_t crtc_id, conn_id;
static struct fbuf buf[2];
static struct drm_mode_modeinfo mode;

static int xioctl(unsigned long r, void *a) {
    int rc, tries = 0;
    do { rc = ioctl(drm_fd, r, a); } while (rc < 0 && (errno == EINTR || errno == EAGAIN) && ++tries < 50);
    return rc;
}

static void drm_close(void) {
    for (int i = 0; i < 2; i++) if (buf[i].map) munmap(buf[i].map, buf[i].size);
    memset(buf, 0, sizeof buf);
    if (drm_fd >= 0) close(drm_fd);
    drm_fd = -1;
}

static int make_fb(struct fbuf *b, int w, int h) {
    struct drm_mode_create_dumb cd;
    struct drm_mode_fb_cmd2 f;
    struct drm_mode_map_dumb md;
    memset(&cd, 0, sizeof cd); memset(&f, 0, sizeof f); memset(&md, 0, sizeof md);
    cd.width = w; cd.height = h; cd.bpp = 32;
    if (xioctl(DRM_IOCTL_MODE_CREATE_DUMB, &cd) < 0) return -1;
    f.width = w; f.height = h; f.pixel_format = FOURCC_XBGR8888; f.handles[0] = cd.handle; f.pitches[0] = cd.pitch;
    if (xioctl(DRM_IOCTL_MODE_ADDFB2, &f) < 0) return -1;
    md.handle = cd.handle;
    if (xioctl(DRM_IOCTL_MODE_MAP_DUMB, &md) < 0) return -1;
    void *m = mmap(0, cd.size, PROT_READ | PROT_WRITE, MAP_SHARED, drm_fd, md.offset);
    if (m == MAP_FAILED) return -1;
    memset(m, 0, cd.size);
    b->handle = cd.handle; b->pitch = cd.pitch; b->fb_id = f.fb_id; b->size = cd.size; b->map = m;
    return 0;
}

/* one attempt: open, pick the connected connector and its CRTC, two buffers, SETCRTC on a black one; on failure the device is closed */
static int drm_attempt(int n) {
    drm_fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
    if (drm_fd < 0) { flog("display %d: open errno=%d", n, errno); return -1; }
    uint32_t crtcs[8] = {0}, conns[8] = {0}, encs[8] = {0};
    struct drm_mode_card_res res;
    memset(&res, 0, sizeof res);
    xioctl(DRM_IOCTL_MODE_GETRESOURCES, &res); /* counts */
    res.count_crtcs = res.count_crtcs > 8 ? 8 : res.count_crtcs;
    res.count_connectors = res.count_connectors > 8 ? 8 : res.count_connectors;
    res.count_encoders = res.count_encoders > 8 ? 8 : res.count_encoders;
    res.crtc_id_ptr = (uintptr_t)crtcs; res.connector_id_ptr = (uintptr_t)conns; res.encoder_id_ptr = (uintptr_t)encs;
    res.fb_id_ptr = 0; res.count_fbs = 0;
    if (xioctl(DRM_IOCTL_MODE_GETRESOURCES, &res) < 0) { flog("display %d: GETRESOURCES errno=%d", n, errno); drm_close(); return -1; }

    uint32_t enc_id = 0;
    struct drm_mode_modeinfo modes[16];
    int nmodes = 0;
    conn_id = 0;
    for (uint32_t i = 0; i < res.count_connectors && !conn_id; i++) {
        struct drm_mode_get_connector probe, gc;
        uint32_t ee[8];
        memset(&probe, 0, sizeof probe); memset(&gc, 0, sizeof gc);
        probe.connector_id = conns[i];
        xioctl(DRM_IOCTL_MODE_GETCONNECTOR, &probe); /* the first pass makes the kernel probe the modes */
        gc.connector_id = conns[i];
        gc.modes_ptr = (uintptr_t)modes; gc.count_modes = probe.count_modes > 16 ? 16 : probe.count_modes;
        gc.encoders_ptr = (uintptr_t)ee; gc.count_encoders = probe.count_encoders > 8 ? 8 : probe.count_encoders;
        if (xioctl(DRM_IOCTL_MODE_GETCONNECTOR, &gc) < 0 || gc.connection != 1 || !gc.count_modes) continue;
        conn_id = conns[i]; enc_id = gc.encoder_id; nmodes = gc.count_modes > 16 ? 16 : gc.count_modes;
    }
    if (!conn_id) { flog("display %d: no connected connector with modes", n); drm_close(); return -1; }
    int sel = 0;
    for (int i = 0; i < nmodes; i++) if (modes[i].type & DRM_MODE_TYPE_PREFERRED) { sel = i; break; }
    mode = modes[sel];
    if (mode.hdisplay < W || mode.vdisplay < H) { flog("display %d: mode %ux%u is smaller than the UI", n, mode.hdisplay, mode.vdisplay); drm_close(); return -1; }

    crtc_id = 0;
    if (enc_id) {
        struct drm_mode_get_encoder e;
        memset(&e, 0, sizeof e);
        e.encoder_id = enc_id;
        if (xioctl(DRM_IOCTL_MODE_GETENCODER, &e) == 0) crtc_id = e.crtc_id;
    }
    if (!crtc_id) crtc_id = crtcs[0];
    struct drm_mode_crtc c0;
    memset(&c0, 0, sizeof c0);
    c0.crtc_id = crtc_id;
    xioctl(DRM_IOCTL_MODE_GETCRTC, &c0);
    flog("display %d: connector %u crtc %u mode %ux%u, crtc fb=%u mode_valid=%u", n, conn_id, crtc_id, mode.hdisplay, mode.vdisplay, c0.fb_id, c0.mode_valid);

    if (make_fb(&buf[0], mode.hdisplay, mode.vdisplay) < 0 || make_fb(&buf[1], mode.hdisplay, mode.vdisplay) < 0) {
        flog("display %d: buffers failed errno=%d", n, errno); drm_close(); return -1;
    }
    for (uint32_t i = 0; i < res.count_crtcs; i++) /* like the stock backend: every other CRTC off */
        if (crtcs[i] != crtc_id) { struct drm_mode_crtc o; memset(&o, 0, sizeof o); o.crtc_id = crtcs[i]; xioctl(DRM_IOCTL_MODE_SETCRTC, &o); }
    struct drm_mode_crtc sc;
    memset(&sc, 0, sizeof sc);
    sc.crtc_id = crtc_id; sc.fb_id = buf[1].fb_id; sc.set_connectors_ptr = (uintptr_t)&conn_id; sc.count_connectors = 1;
    sc.mode_valid = 1; sc.mode = mode;
    if (xioctl(DRM_IOCTL_MODE_SETCRTC, &sc) < 0) { flog("display %d: SETCRTC errno=%d", n, errno); drm_close(); return -1; }
    pitch_px = buf[0].pitch / 4; cur = 0; fb = buf[0].map; swap_rb = 1;
    return 0;
}

static int display_init(void) {
    if (sim) { pitch_px = W; fb = calloc(W * H, 4); swap_rb = 0; disp_tries = 1; return 1; }
    for (int i = 0; i < 100 && access("/dev/dri/card0", F_OK); i++) usleep(100000);
    for (int n = 1; n <= 3; n++) {
        if (drm_attempt(n) == 0) { disp_tries = n; flog("display: ok after %d attempt(s)", n); return n; }
        beat(0);
    }
    return 0;
}

/* SETCRTC(off) + close: the lastclose then leaves the pipeline disabled, so the next user (stock recovery, bootloader) starts clean */
static void display_off(void) {
    if (drm_fd < 0) return;
    struct drm_mode_crtc off;
    memset(&off, 0, sizeof off);
    off.crtc_id = crtc_id;
    int r = xioctl(DRM_IOCTL_MODE_SETCRTC, &off);
    flog("display off: SETCRTC(off) -> %d", r);
    drm_close();
}

static void dump_frame(void) {
    static int n;
    static uint32_t last_sum;
    if (!dump_dir || n >= 40) return;
    uint32_t sum = 0;
    for (int i = 0; i < W * H; i++) sum = sum * 31 + fb[i];
    if (n && sum == last_sum) return;
    last_sum = sum;
    char p[160];
    snprintf(p, sizeof p, "%s/frame_%03d.ppm", dump_dir, n++);
    FILE *f = fopen(p, "wb");
    if (!f) return;
    fprintf(f, "P6\n%d %d\n255\n", W, H);
    for (int i = 0; i < W * H; i++) { fputc(fb[i] >> 16, f); fputc((fb[i] >> 8) & 255, f); fputc(fb[i] & 255, f); }
    fclose(f);
}

static void present(void) {
    if (sim) { dump_frame(); return; }
    if (drm_fd < 0) return;
    struct drm_mode_crtc_page_flip pf;
    memset(&pf, 0, sizeof pf);
    pf.crtc_id = crtc_id; pf.fb_id = buf[cur].fb_id; pf.flags = DRM_MODE_PAGE_FLIP_EVENT;
    int r = xioctl(DRM_IOCTL_MODE_PAGE_FLIP, &pf), e = errno;
    if (r < 0) { /* no flip: show the frame with a modeset instead */
        struct drm_mode_crtc c;
        memset(&c, 0, sizeof c);
        c.crtc_id = crtc_id; c.fb_id = buf[cur].fb_id; c.set_connectors_ptr = (uintptr_t)&conn_id; c.count_connectors = 1; c.mode_valid = 1; c.mode = mode;
        int r2 = xioctl(DRM_IOCTL_MODE_SETCRTC, &c);
        if (!flip_logged++) flog("display: page flip errno=%d, SETCRTC fallback -> %d", e, r2);
        if (r2 < 0) return;
    } else {
        struct pollfd p = {.fd = drm_fd, .events = POLLIN};
        if (poll(&p, 1, 300) > 0) { char ev[256]; read(drm_fd, ev, sizeof ev); }
    }
    cur ^= 1;
    fb = buf[cur].map;
}

/* ---------------- inputs: every non-touch key device, so a numbering difference in recovery does not matter ---------------- */
static struct pollfd pf[MAXIN];
static char inname[MAXIN][32];

static void open_inputs(void) {
    for (int i = 0; i < 16 && nin < MAXIN; i++) {
        char p[32], name[32] = {0};
        unsigned long types[1] = {0}, absb[(ABS_MAX + LBITS) / LBITS];
        snprintf(p, sizeof p, "/dev/input/event%d", i);
        int fd = open(p, O_RDONLY | O_CLOEXEC);
        if (fd < 0) continue;
        memset(absb, 0, sizeof absb);
        ioctl(fd, EVIOCGNAME(sizeof name - 1), name);
        ioctl(fd, EVIOCGBIT(0, sizeof types), types);
        ioctl(fd, EVIOCGBIT(EV_ABS, sizeof absb), absb);
        int key = (types[0] >> EV_KEY) & 1, touch = (absb[ABS_MT_POSITION_Y / LBITS] >> (ABS_MT_POSITION_Y % LBITS)) & 1;
        flog("input event%d '%s' key=%d touch=%d", i, name, key, touch);
        if (!key || touch) { close(fd); continue; }
        pf[nin].fd = fd; pf[nin].events = POLLIN;
        snprintf(inname[nin], sizeof inname[nin], "%s", name);
        nin++;
    }
}

/* ---------------- menu model ---------------- */
struct target { char label[20], path[96]; };
static struct target targets[MAXT];
static int ntargets;
static struct item main_items[] = {
    {"WEAR OS", A_WEAROS, 0, 1}, {"DUAL BOOT", A_DUAL, 0, 1}, {"RECOVERY STOCK", A_STOCK, 0, 1},
    {"FASTBOOT", A_FASTBOOT, 0, 1}, {"INFO", A_INFO, 0, 1}, {"SPEGNI", A_OFF, 0, 1},
};
static struct item dual_items[MAXT + 1];
static int ndual, screen = S_MAIN, sel, top, saved_sel;

/* sn0w-targets.conf: "LABEL|/path/to/payload" per line, '#' starts a comment; the label is shown in capitals, 16 characters at most */
static void load_targets(void) {
    FILE *f = fopen(conf_path, "r");
    char l[160];
    if (!f) flog("targets: cannot open %s errno=%d", conf_path, errno);
    while (f && ntargets < MAXT && fgets(l, sizeof l, f)) {
        l[strcspn(l, "\r\n")] = 0;
        char *bar = strchr(l, '|');
        if (l[0] == '#' || !bar) continue;
        *bar = 0;
        struct target *t = &targets[ntargets];
        snprintf(t->label, 17, "%s", l);
        for (char *c = t->label; *c; c++) if (*c >= 'a' && *c <= 'z') *c -= 32;
        snprintf(t->path, sizeof t->path, "%s", bar + 1);
        ntargets++;
    }
    if (f) fclose(f);
    for (int i = 0; i < ntargets; i++) {
        dual_items[i].label = targets[i].label; dual_items[i].act = A_TARGET; dual_items[i].arg = i;
        dual_items[i].enabled = access(targets[i].path, X_OK) == 0;
        flog("target %d '%s' -> %s %s", i, targets[i].label, targets[i].path, dual_items[i].enabled ? "(present)" : "(missing)");
    }
    dual_items[ntargets].label = "INDIETRO"; dual_items[ntargets].act = A_BACK; dual_items[ntargets].enabled = 1;
    ndual = ntargets + 1;
}

static void drain_inputs(void) { /* presses made while a payload was running must not act afterwards */
    for (int k = 0; k < nin; k++) {
        struct input_event ev[8];
        struct pollfd p = {.fd = pf[k].fd, .events = POLLIN};
        while (poll(&p, 1, 0) > 0 && read(p.fd, ev, sizeof ev) > 0) ;
    }
}

static void notice(int secs, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(notice_buf, sizeof notice_buf, fmt, ap);
    va_end(ap);
    notice_until = mono() + secs;
    dirty = 1;
}

static void info_lines(char ln[][32], const char **p) {
    char b[32];
    int ch = 0, cap = batt(&ch), n = 0;
    long up = 0;
    if (rd("/proc/uptime", b, sizeof b) > 0) up = atol(b);
    if (cap >= 0) snprintf(ln[n], 32, "BATT %d%% %s", cap, ch ? "IN CARICA" : "A BATTERIA"); else snprintf(ln[n], 32, "BATT ?");
    snprintf(ln[++n], 32, "UP %ldS  BUILD %s", up, BUILD);
    snprintf(ln[++n], 32, "BCB IN: %s", bcb_in);
    snprintf(ln[++n], 32, "DISPLAY OK TENT %d", disp_tries);
    snprintf(ln[++n], 32, "TASTI: %d DISPOSITIVI", nin);
    snprintf(ln[++n], 32, failed_label[0] ? "FALLITO: %.14s" : "DUAL BOOT: OK", failed_label);
    for (int i = 0; i <= n; i++) p[i] = ln[i];
}

/* ---------------- leaving ---------------- */
static void leave(const char *ctl, const char *why) {
    flog("LEAVE %s (%s)", ctl, why);
    bcb_write(0);
    mark_set(NULL);
    display_off();
    if (sim) { flog("SIM: sys.powerctl %s", ctl); exit(0); }
    sync();
    int r = __system_property_set("sys.powerctl", ctl);
    flog("sys.powerctl '%s' -> %d", ctl, r);
    for (;;) sleep(60); /* init takes the watch down; the heartbeat has stopped, so the deadman covers a request that is not honoured */
}

static void draw(const char *status) {
    const char *lv[LOG_LINES];
    int nlv = ring_view(lv, LOG_LINES);
    if (screen == S_INFO) {
        char ln[6][32];
        const char *p[6];
        info_lines(ln, p);
        draw_lines(p, 6, status);
    } else if (screen == S_DUAL) draw_menu(dual_items, ndual, sel, top, status, lv, nlv);
    else draw_menu(main_items, 6, sel, top, status, lv, nlv);
}

static void run_stock(void) {
    flog("STOCK recovery: deadman pinned, exec");
    bcb_write(0);
    mark_set(NULL);
    beat(1);
    display_off();
    if (sim) { flog("SIM: exec %s", STOCK); exit(0); }
    execl(STOCK, "recovery", (char *)0);
    flog("exec stock failed errno=%d", errno);
    beat(0);
    if (!display_init()) leave("reboot,", "stock exec failed and no display");
    notice(4, "STOCK NON AVVIATA");
}

static void pump(int fd, char *acc, int *na, int *nl) { /* payload output, line by line */
    char b[128];
    struct pollfd p = {.fd = fd, .events = POLLIN};
    while (poll(&p, 1, 0) > 0) {
        ssize_t r = read(fd, b, sizeof b);
        if (r <= 0) break;
        for (ssize_t i = 0; i < r; i++) {
            if (b[i] == '\n' || *na == 158) {
                acc[*na] = 0;
                if (*na) { if ((*nl)++ < 30) flog("> %s", acc); else vlog("> %s", acc); }
                *na = 0;
                if (b[i] == '\n') continue;
            }
            acc[(*na)++] = b[i];
        }
    }
}

/* runs the payload as a child while the heartbeat and the screen stay alive; the BCB is set meanwhile so a payload that resets
 * the watch lands back here (the next start reports it and clears the BCB again) */
static void run_target(int k) {
    const struct target *t = &targets[k];
    if (access(t->path, X_OK)) { flog("target '%s': payload %s missing", t->label, t->path); notice(3, "NESSUN PAYLOAD"); return; }
    mark_set(t->label);
    bcb_write(1);
    flog("LAUNCH '%s' %s", t->label, t->path);
    int pfd[2];
    if (pipe2(pfd, O_CLOEXEC)) pfd[0] = pfd[1] = -1;
    pid_t pid = fork();
    if (pid == 0) {
        if (pfd[1] >= 0) { dup2(pfd[1], 1); dup2(pfd[1], 2); }
        execl(t->path, t->path, (char *)0);
        _exit(127);
    }
    if (pfd[1] >= 0) close(pfd[1]);
    if (pid < 0) {
        flog("fork failed errno=%d", errno);
        if (pfd[0] >= 0) close(pfd[0]);
        bcb_write(0); mark_set(NULL); notice(3, "FORK FALLITO");
        return;
    }
    long start = mono();
    int st = 0, done = 0, nl = 0, na = 0;
    char shown[32], acc[160];
    snprintf(shown, sizeof shown, "AVVIO %s", t->label);
    while (!done) {
        beat(0);
        last_input = mono();
        if (dirty) { draw(shown); dirty = 0; }
        if (waitpid(pid, &st, WNOHANG) == pid) done = 1;
        else if (mono() - start > payload_max_s) { flog("payload timeout: killing it"); kill(pid, SIGKILL); waitpid(pid, &st, 0); st = -1; done = 1; }
        if (pfd[0] >= 0) {
            struct pollfd p = {.fd = pfd[0], .events = POLLIN};
            if (!done) poll(&p, 1, 300);
            pump(pfd[0], acc, &na, &nl);
        } else if (!done) usleep(300000);
    }
    if (na) { acc[na] = 0; vlog("> %s", acc); }
    if (pfd[0] >= 0) close(pfd[0]);
    int code = st == -1 ? -1 : WIFEXITED(st) ? WEXITSTATUS(st) : 128 + WTERMSIG(st);
    flog("payload '%s' returned %d", t->label, code);
    bcb_write(0);
    mark_set(NULL);
    drain_inputs();
    last_input = mono();
    notice(6, "PAYLOAD FINITO: %d", code);
}

static void activate(const struct item *it) {
    switch (it->act) {
    case A_WEAROS: leave("reboot,", "wear os"); break;
    case A_DUAL: saved_sel = sel; screen = S_DUAL; sel = top = 0; if (!ntargets) notice(4, "NESSUN TARGET"); break;
    case A_STOCK: run_stock(); break;
    case A_FASTBOOT: leave("reboot,bootloader", "fastboot"); break;
    case A_INFO: screen = S_INFO; break;
    case A_OFF: leave("shutdown,", "power off"); break;
    case A_BACK: screen = S_MAIN; sel = saved_sel; top = sel >= ROWS ? sel - ROWS + 1 : 0; break;
    case A_TARGET: run_target(it->arg); break;
    }
    dirty = 1;
}

static void on_key(int code, const char *dev) {
    static int logged;
    if (logged++ < (keylog ? 400 : 40)) flog("key %s code=%d", dev, code); else vlog("key %s code=%d", dev, code);
    if (keylog) return;
    last_input = mono();
    dirty = 1;
    if (screen == S_INFO) { screen = S_MAIN; return; }
    const struct item *items = screen == S_DUAL ? dual_items : main_items;
    int n = screen == S_DUAL ? ndual : 6;
    if (code == KEY_POWER) { activate(&items[sel]); return; }
    sel = (sel + 1) % n;
    if (sel < top) top = sel;
    if (sel >= top + ROWS) top = sel - ROWS + 1;
}

static const char *status_text(char *b, size_t n) {
    long left = idle_s - (mono() - last_input);
    if (mono() < notice_until) return notice_buf;
    if (screen == S_INFO) return "OGNI TASTO = INDIETRO";
    if (left <= 30) { snprintf(b, n, "RIAVVIO AUTO TRA %lds", left > 0 ? left : 0); return b; }
    return "PWR=OK  ALTRI=AVANTI";
}

static int ioption(const char *a, int *v, int *i, int argc, char **argv, const char *name) {
    if (strcmp(a, name) || *i + 1 >= argc) return 0;
    *v = atoi(argv[++*i]);
    return 1;
}

int main(int argc, char **argv) {
    int deadman = 0;
    for (int i = 1; i < argc; i++) {
        const char *a = argv[i];
        if (!strcmp(a, "--deadman")) deadman = 1;
        else if (!strcmp(a, "--sim")) sim = 1;
        else if (!strcmp(a, "--real-inputs")) real_inputs = 1;
        else if (!strcmp(a, "--keylog")) sim = real_inputs = keylog = 1;
        else if (!strcmp(a, "--misc") && i + 1 < argc) misc_path = argv[++i];
        else if (!strcmp(a, "--targets") && i + 1 < argc) conf_path = argv[++i];
        else if (!strcmp(a, "--wd") && i + 1 < argc) wd_path = argv[++i];
        else if (!strcmp(a, "--batt-dir") && i + 1 < argc) batt_dir = argv[++i];
        else if (!strcmp(a, "--dump") && i + 1 < argc) dump_dir = argv[++i];
        else if (!strcmp(a, "--keys") && i + 1 < argc) keys = argv[++i];
        else if (ioption(a, &idle_s, &i, argc, argv, "--idle") || ioption(a, &slack_s, &i, argc, argv, "--slack") ||
                 ioption(a, &stuck_s, &i, argc, argv, "--stuck") || ioption(a, &grace_s, &i, argc, argv, "--grace") ||
                 ioption(a, &cap_s, &i, argc, argv, "--cap") || ioption(a, &batt_min, &i, argc, argv, "--batt-min") ||
                 ioption(a, &payload_max_s, &i, argc, argv, "--payload-max") || ioption(a, &hang_s, &i, argc, argv, "--hang")) continue;
    }
    t0 = mono();
    if (deadman) return deadman_main();

    flog("=== sn0wmenu %s pid=%d sim=%d idle=%d", BUILD, (int)getpid(), sim, idle_s);
    char c[32] = {0};
    if (misc_read(0, c, 24) == 0) {
        for (int i = 0; i < 23 && c[i]; i++) bcb_in[i] = c[i] >= 'a' && c[i] <= 'z' ? c[i] - 32 : c[i] >= ' ' && c[i] < 127 ? c[i] : '?';
    }
    if (!bcb_in[0]) strcpy(bcb_in, "VUOTO");
    flog("BCB at entry: '%s'", bcb_in);
    bcb_write(0); /* fail-open: from here on any crash or hardware reset boots Wear OS */
    if (mark_get(failed_label, sizeof failed_label)) { flog("the previous dual boot attempt '%s' did not come back cleanly", failed_label); mark_set(NULL); notice(8, "TENTATIVO FALLITO"); }
    if (!sim) flog("adb: sys.usb.config=adb -> %d", __system_property_set("sys.usb.config", "adb")); /* the host can always run `adb reboot` */
    last_input = mono();
    beat(0);
    load_targets();
    if (!display_init()) leave("reboot,", "no display");
    if (!sim || real_inputs) open_inputs();
    flog("ready: %d key device(s), %d target(s)", nin, ntargets);

    char shown[32] = "", sb[32];
    long next_key = mono_ms() + 600, last_draw = 0;
    const char *kp = sim ? keys : NULL;
    for (;;) {
        long now = mono();
        beat(0);
        if (hang_s && now - t0 >= hang_s) { flog("TEST: the menu freezes"); for (;;) sleep(100); }
        if (now - last_input >= idle_s) leave("reboot,", "idle timeout");
        const char *st = status_text(sb, sizeof sb);
        if (dirty || strcmp(st, shown) || (screen == S_INFO && now != last_draw)) {
            draw(st);
            snprintf(shown, sizeof shown, "%s", st);
            dirty = 0;
            last_draw = now;
        }
        if (kp && *kp && mono_ms() >= next_key) { /* --keys */
            char k = *kp++;
            next_key = mono_ms() + (k == 'w' ? 1000 : 250);
            if (k == 'n') on_key(KEY_F5, "script");
            else if (k == 'p') on_key(KEY_POWER, "script");
        }
        if (poll(pf, nin, sim ? 100 : 500) <= 0) continue;
        for (int k = 0; k < nin; k++) {
            struct input_event ev[8];
            if (!(pf[k].revents & POLLIN)) continue;
            ssize_t n = read(pf[k].fd, ev, sizeof ev);
            if (n < (ssize_t)sizeof ev[0]) { static int once; if (!once++) flog("input read -> %d errno=%d (event size %d)", (int)n, errno, (int)sizeof ev[0]); continue; }
            for (int j = 0; j < n / (int)sizeof ev[0]; j++)
                if (ev[j].type == EV_KEY && ev[j].value == 1) on_key(ev[j].code, inname[k]);
        }
    }
}
