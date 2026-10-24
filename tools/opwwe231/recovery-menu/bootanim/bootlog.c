/* sn0w bootlog: live verbose window for the Wear OS boot animation (static ARM32, runs as root from the Magisk module).
 *
 * The stock player decodes each frame from the zip at the moment it shows it (parts with count 1), and it maps the zip, so bytes
 * rewritten inside the zip file just before a frame is shown change what is on screen (checked on the watch: red frames turned
 * green mid-animation). mkbootanim.py builds a ring of K stored palette PNG frames that many parts replay; every frame has the
 * same size, so only the pixel data, the zlib checksum and the chunk CRC change. This program keeps the last kernel log lines and
 * refreshes the ring slot that will be shown about 1.2 s from now. The logo is static, only the console text moves.
 *
 *   bootlog --module DIR [--zip FILE] [--test] [--t0-now] [--max SEC] [--verbose]
 * DIR holds bootlog/idx ("W H FPS K" then "offset length" per slot), bootlog/base.bin (W*H palette indices: the logo on black)
 * and bootlog/pal.bin (256 RGB entries). --test ignores the boot properties, --t0-now starts the schedule at once
 * instead of waiting for the player's "BootAnimationShownTiming" log line.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/system_properties.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "font9.h"
#include "logo_boot.h"
#include "boot_ui.h"

#define KEEP 64           /* kernel lines remembered */
#define AHEAD_MS 1200     /* a slot is refreshed this long before it is shown */

static void present(void) {}

static long clock_ms(clockid_t c) { struct timespec t; clock_gettime(c, &t); return t.tv_sec * 1000L + t.tv_nsec / 1000000L; }
static long mono_ms(void) { return clock_ms(CLOCK_MONOTONIC); }

/* ---------------- PNG pieces ---------------- */
static uint32_t crc_tab[256];
static void crc_init(void) {
    for (uint32_t n = 0; n < 256; n++) { uint32_t c = n; for (int k = 0; k < 8; k++) c = c & 1 ? 0xEDB88320u ^ (c >> 1) : c >> 1; crc_tab[n] = c; }
}
static uint32_t crc_update(uint32_t c, const uint8_t *p, size_t n) { while (n--) c = crc_tab[(c ^ *p++) & 255] ^ (c >> 8); return c; }

static uint32_t adler32(const uint8_t *p, size_t n) {
    uint32_t a = 1, b = 0;
    while (n) {
        size_t m = n < 5552 ? n : 5552;
        n -= m;
        while (m--) { a += *p++; b += a; }
        a %= 65521; b %= 65521;
    }
    return b << 16 | a;
}

/* zlib stream with stored blocks for the filtered scanlines, followed by the IDAT chunk CRC; same layout as pnglive.py */
static size_t make_payload(const uint8_t *px, uint8_t *raw, uint8_t *out) {
    size_t rawlen = (size_t)H * (W + 1), o = 0;
    for (int y = 0; y < H; y++) { raw[(size_t)y * (W + 1)] = 0; memcpy(raw + (size_t)y * (W + 1) + 1, px + (size_t)y * W, W); }
    out[o++] = 0x78; out[o++] = 0x01;
    for (size_t pos = 0; pos < rawlen; pos += 65535) {
        size_t n = rawlen - pos < 65535 ? rawlen - pos : 65535;
        out[o++] = pos + n >= rawlen;
        out[o++] = n & 255; out[o++] = n >> 8; out[o++] = ~n & 255; out[o++] = (~n >> 8) & 255;
        memcpy(out + o, raw + pos, n);
        o += n;
    }
    uint32_t ad = adler32(raw, rawlen);
    out[o++] = ad >> 24; out[o++] = ad >> 16; out[o++] = ad >> 8; out[o++] = ad;
    uint32_t c = crc_update(0xFFFFFFFFu, (const uint8_t *)"IDAT", 4);
    c = ~crc_update(c, out, o);
    size_t len = o;
    out[o++] = c >> 24; out[o++] = c >> 16; out[o++] = c >> 8; out[o++] = c;
    return len;
}

/* ---------------- state ---------------- */
static int fps, K, zfd = -1, verbose;
static struct { off_t off; size_t len; } *slot;
static uint8_t *base, *frame, *raw, *payload;
static uint8_t pal[768];
static uint32_t *rgb;
static char klines[KEEP][CON_COLS + 1];
static uint8_t box_bg; /* palette index of BOX_BG */
static int nk;

static uint8_t lookup(uint32_t c) { /* palette index of a window colour; exact match, nearest otherwise */
    static uint32_t cc[64];
    static uint8_t ci[64];
    static uint8_t used[64];
    unsigned h = (c * 2654435761u) >> 26;
    if (used[h] && cc[h] == c) return ci[h];
    int best = 0, bd = 1 << 30;
    for (int i = 0; i < 256; i++) {
        int dr = pal[3 * i] - (int)(c >> 16 & 255), dg = pal[3 * i + 1] - (int)(c >> 8 & 255), db = pal[3 * i + 2] - (int)(c & 255);
        int d = dr * dr + dg * dg + db * db;
        if (d < bd) { bd = d; best = i; }
    }
    used[h] = 1; cc[h] = c; ci[h] = best;
    return best;
}

static void add_line(const char *s) {
    char *d = klines[nk++ % KEEP];
    int n = 0;
    for (; *s && n < CON_COLS; s++) d[n++] = *s >= 'a' && *s <= 'z' ? *s - 32 : (*s >= ' ' && *s < 127) ? *s : '?';
    d[n] = 0;
}

/* /dev/kmsg: one record per read, "pri,seq,usec,flags;message\n[ key=value\n...]" */
static int kmsg_drain(int fd) {
    char b[8192];
    int got = 0;
    for (;;) {
        ssize_t r = read(fd, b, sizeof b - 1);
        if (r < 0) { if (errno == EPIPE) continue; break; } /* EPIPE: records were overwritten, the next read goes on */
        if (r == 0) break;
        b[r] = 0;
        char *semi = strchr(b, ';');
        unsigned long long usec = 0;
        if (!semi || sscanf(b, "%*d,%*llu,%llu,", &usec) != 1) continue;
        char *e = strchr(semi, '\n');
        if (e) *e = 0;
        char line[160];
        snprintf(line, sizeof line, "%llu.%02llu %s", usec / 1000000, (usec / 10000) % 100, semi + 1);
        add_line(line);
        got++;
    }
    return got;
}

static void render(void) { /* the console layer under the logo: text shows only on the box background */
    memcpy(frame, base, (size_t)W * H);
    for (size_t i = 0; i < (size_t)W * H; i++) rgb[i] = CON_NONE;
    const char *lv[CON_ROWS];
    int n = nk < CON_ROWS ? nk : CON_ROWS;
    for (int i = 0; i < n; i++) lv[i] = klines[(nk - n + i) % KEEP];
    draw_console(lv, n);
    for (size_t i = 0; i < (size_t)W * H; i++)
        if (rgb[i] != CON_NONE && base[i] == box_bg) frame[i] = lookup(rgb[i]);
}

static int write_slot(int k) {
    size_t len = make_payload(frame, raw, payload);
    if (len != slot[k].len) { fprintf(stderr, "bootlog: slot %d payload %zu B, expected %zu B\n", k, len, slot[k].len); return -1; }
    if (pwrite(zfd, payload, len + 4, slot[k].off) != (ssize_t)(len + 4)) { fprintf(stderr, "bootlog: pwrite errno=%d\n", errno); return -1; }
    return 0;
}

static int load(const char *path, void *buf, size_t n) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) { fprintf(stderr, "bootlog: cannot open %s errno=%d\n", path, errno); return -1; }
    ssize_t r = read(fd, buf, n);
    close(fd);
    if (r != (ssize_t)n) { fprintf(stderr, "bootlog: %s: %zd bytes, expected %zu\n", path, r, n); return -1; }
    return 0;
}

/* When part 0 starts the player logs "Playing files = <zip>/live, Requested repeat = 1" and shows its first frame a few ms later. Returns the
 * age of that first line in ms (the one for part 0; later parts log the same text), or -1 if there is none yet. logcat -d dumps and exits,
 * so its output is not held back in a pipe buffer (a following logcat child was); -v epoch avoids time zones. */
static long player_age_ms(void) {
    FILE *p = popen("/system/bin/logcat -d -v epoch -b main -s BootAnimation:D 2>/dev/null", "r");
    if (!p) return -1;
    char l[300];
    long age = -1;
    int lines = 0;
    while (fgets(l, sizeof l, p)) {
        long sec, ms;
        lines++;
        if (age < 0 && strstr(l, "Playing files =") && sscanf(l, "%ld.%ld", &sec, &ms) == 2) {
            struct timespec now;
            clock_gettime(CLOCK_REALTIME, &now);
            age = (now.tv_sec - sec) * 1000L + now.tv_nsec / 1000000L - ms;
        }
    }
    pclose(p);
    if (verbose) fprintf(stderr, "bootlog: logcat poll: %d lines, first part started %ld ms ago\n", lines, age);
    return age;
}

int main(int argc, char **argv) {
    const char *dir = 0, *zip = 0;
    int test = 0, t0_now = 0, max_s = 300;
    verbose = 0;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--module") && i + 1 < argc) dir = argv[++i];
        else if (!strcmp(argv[i], "--zip") && i + 1 < argc) zip = argv[++i];
        else if (!strcmp(argv[i], "--test")) test = 1;
        else if (!strcmp(argv[i], "--t0-now")) t0_now = 1;
        else if (!strcmp(argv[i], "--verbose")) verbose = 1;
        else if (!strcmp(argv[i], "--max") && i + 1 < argc) max_s = atoi(argv[++i]);
    }
    if (!dir) { fprintf(stderr, "usage: bootlog --module DIR [--zip FILE] [--test] [--t0-now] [--max SEC]\n"); return 2; }
    char p[256], zp[256];
    snprintf(p, sizeof p, "%s/bootlog/idx", dir);
    FILE *f = fopen(p, "r");
    int w, h;
    if (!f || fscanf(f, "%d %d %d %d", &w, &h, &fps, &K) != 4 || w != W || h != H || K < 8 || K > 200 || fps < 1) { fprintf(stderr, "bootlog: bad idx %s\n", p); return 1; }
    slot = calloc(K, sizeof *slot);
    for (int k = 0; k < K; k++) { long long o; unsigned l; if (fscanf(f, "%lld %u", &o, &l) != 2) { fprintf(stderr, "bootlog: short idx\n"); return 1; } slot[k].off = o; slot[k].len = l; }
    fclose(f);
    base = malloc((size_t)W * H); frame = malloc((size_t)W * H); raw = malloc((size_t)H * (W + 1)); payload = malloc((size_t)H * (W + 1) + 64);
    rgb = calloc((size_t)W * H, 4);
    snprintf(p, sizeof p, "%s/bootlog/base.bin", dir);
    if (load(p, base, (size_t)W * H)) return 1;
    snprintf(p, sizeof p, "%s/bootlog/pal.bin", dir);
    if (load(p, pal, sizeof pal)) return 1;
    snprintf(zp, sizeof zp, "%s", zip ? zip : "");
    if (!zip) snprintf(zp, sizeof zp, "%s/system/media/bootanimation.zip", dir);
    zfd = open(zp, O_RDWR);
    if (zfd < 0) { fprintf(stderr, "bootlog: cannot open %s errno=%d\n", zp, errno); return 1; }
    crc_init();
    box_bg = lookup(BOX_BG);
    pitch_px = W; fb = rgb; swap_rb = 0;

    int kfd = open("/dev/kmsg", O_RDONLY | O_NONBLOCK);
    if (kfd < 0) fprintf(stderr, "bootlog: /dev/kmsg errno=%d\n", errno);
    else kmsg_drain(kfd);
    render();
    for (int k = 0; k < K; k++) write_slot(k);
    fprintf(stderr, "bootlog: %d slots primed, %d kernel lines\n", K, nk);

    long t0 = t0_now ? mono_ms() : -1, start = mono_ms(), last_full = start, last_poll = 0;
    long done_at = -1;
    int last_slot = -1;
    for (;;) {
        struct pollfd pf = {.fd = kfd, .events = POLLIN};
        poll(&pf, 1, 20);
        long now = mono_ms();
        int fresh = kfd >= 0 ? kmsg_drain(kfd) : 0;
        if (t0 < 0 && now - last_poll >= 250) {
            long age = player_age_ms();
            last_poll = mono_ms();
            if (age >= 0 && age < 30000) { t0 = last_poll - age; fprintf(stderr, "bootlog: player started %ld ms ago\n", age); }
        }
        if (t0 < 0 && now - start > 45000) { t0 = now; fprintf(stderr, "bootlog: no player event, assuming it started now\n"); }
        if (t0 < 0) { /* the player has not started: keep every slot fresh */
            if (fresh && now - last_full >= 1000) { render(); for (int k = 0; k < K; k++) write_slot(k); last_full = now; }
        } else {
            long n = (now - t0 + AHEAD_MS) * fps / 1000; /* absolute frame number that will be shown AHEAD_MS from now */
            if (n < 0) n = 0;
            if (n > last_slot || last_slot < 0) {
                render();
                long from = last_slot < 0 || n - last_slot > K ? n : last_slot + 1;
                for (long s = from; s <= n; s++) write_slot((int)(s % K));
                last_slot = (int)n; /* absolute number, compared as such */
            }
        }
        if (!test) {
            char v[PROP_VALUE_MAX] = {0};
            __system_property_get("sys.boot_completed", v);
            if (done_at < 0 && v[0] == '1') done_at = now;
        }
        if (done_at >= 0 && now - done_at > 4000) { fprintf(stderr, "bootlog: boot completed, leaving\n"); break; }
        if (now - start > max_s * 1000L) { fprintf(stderr, "bootlog: time limit, leaving\n"); break; }
    }
    return 0;
}
