/* boot-watchos: payload for the sn0wbite recovery DUAL BOOT menu.
 *
 * Runs as root (recovery context, Magisk-equivalent) when the user picks
 * WATCHOS TEST from sn0wmenu9's DUAL BOOT menu. Target path (per
 * sn0w-targets.conf): /sbin/boot-watchos. sn0wmenu9 forks us, pipes stdout
 * line-by-line to the display verbose window, and shows the exit code at
 * the end. Also everything we write to stdout is captured in the flight
 * recorder (misc@0x9000) and readable from Wear OS via read_flightlog.sh.
 *
 * v0 is deliberately minimal — a "proof of channel". We print identity +
 * a handful of /proc values the sn0wbite recovery / Wear OS probes already
 * confirm, so a successful run is "sn0wmenu picked us, ran us, we spoke
 * back". Later versions add real diagnostic work: ID_AA64 probe (via a
 * tiny kernel module), MMIO mapping candidates for the Apple AIC/PMGR
 * shadow, ramoops record format writing, boot_args hex dump via mk_bootargs.
 *
 * Build: static ARM32 (recovery is armv8l / 32-bit userspace per cpuinfo).
 * See the Makefile.
 */

#define _GNU_SOURCE
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>

#define BUILD "v1"

static void dump_file(const char *path, int max_lines) {
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        printf("  %s: errno=%d\n", path, errno);
        return;
    }
    char buf[4096];
    ssize_t n = read(fd, buf, sizeof buf - 1);
    close(fd);
    if (n <= 0) {
        printf("  %s: empty / read errno=%d\n", path, errno);
        return;
    }
    buf[n] = 0;
    int lines = 0;
    for (char *p = buf, *nl; p && *p && lines < max_lines; p = nl ? nl + 1 : NULL) {
        nl = strchr(p, '\n');
        if (nl) *nl = 0;
        if (*p) {
            printf("  %s\n", p);
            lines++;
        }
    }
}

static void dump_file_grep(const char *path, const char *needle) {
    FILE *f = fopen(path, "r");
    if (!f) {
        printf("  %s: errno=%d\n", path, errno);
        return;
    }
    char line[512];
    while (fgets(line, sizeof line, f)) {
        char *nl = strchr(line, '\n'); if (nl) *nl = 0;
        if (strstr(line, needle)) printf("  %s\n", line);
    }
    fclose(f);
}

/* grep but stop after `max_matches` hits (dedup for multi-CPU /proc/cpuinfo) */
static void dump_file_grep_once(const char *path, const char *needle, int max_matches) {
    FILE *f = fopen(path, "r");
    if (!f) { printf("  %s: errno=%d\n", path, errno); return; }
    char line[512]; int hits = 0;
    while (fgets(line, sizeof line, f) && hits < max_matches) {
        char *nl = strchr(line, '\n'); if (nl) *nl = 0;
        if (strstr(line, needle)) { printf("  %s\n", line); hits++; }
    }
    fclose(f);
}

/* read a DT property (text or binary), print as string with length prefix */
static void dt_prop(const char *path, int max_bytes) {
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0) { printf("  %s: errno=%d\n", path, errno); return; }
    char buf[4096];
    ssize_t n = read(fd, buf, sizeof buf < (size_t)max_bytes ? sizeof buf : (size_t)max_bytes);
    close(fd);
    if (n <= 0) { printf("  %s: empty\n", path); return; }
    /* trim trailing nul if present (DT strings are nul-terminated) */
    while (n > 0 && buf[n-1] == 0) n--;
    /* keep printable only, replace others with '.' */
    int all_printable = 1;
    for (ssize_t i = 0; i < n; i++) {
        if (buf[i] < 0x20 && buf[i] != 0) { all_printable = 0; break; }
    }
    if (all_printable) {
        printf("  %s = %.*s\n", path, (int)n, buf);
    } else {
        printf("  %s = <binary %zdB>", path, n);
        for (ssize_t i = 0; i < n && i < 32; i++) printf(" %02x", (unsigned char)buf[i]);
        printf("\n");
    }
}

/* Write a sn0w diagnostic marker to misc partition at offset 0xF000. Format:
 *   "SN0W-DIAG" magic (10) + "\n" + payload text up to 2 KB.
 * Readable from Wear OS: dd if=/dev/block/by-name/misc bs=1 skip=61440 count=2048.
 * This persists across the reboot cycle so we can verify payload-wrote-marker. */
static int write_marker(const char *s) {
    int fd = open("/dev/block/by-name/misc", O_WRONLY | O_CLOEXEC);
    if (fd < 0) { printf("  marker: open errno=%d\n", errno); return -1; }
    const off_t off = 0xF000;
    if (lseek(fd, off, SEEK_SET) == (off_t)-1) {
        printf("  marker: seek errno=%d\n", errno); close(fd); return -1;
    }
    char buf[2048];
    int n = snprintf(buf, sizeof buf, "SN0W-DIAG\n%s", s);
    if (n < 0 || (size_t)n > sizeof buf) n = sizeof buf;
    ssize_t w = write(fd, buf, n);
    fsync(fd);
    close(fd);
    printf("  marker: wrote %zd B at misc+0x%lx\n", w, (long)off);
    return 0;
}

static int write_ramoops_tag(void) {
    /* The OPW2 ramoops reserved region at 0x71f00000 is the designated
     * channel for anything that must survive a crash reboot. From rooted
     * Linux we cannot /dev/mem (ro.secure=1), but we CAN read /proc/kallsyms
     * and bounce through debug ioctls — for v0 we just note we would write
     * if we had the channel. v1 adds a tiny kmod. */
    return 0;
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    setvbuf(stdout, NULL, _IOLBF, 0);

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char ts[64]; strftime(ts, sizeof ts, "%Y-%m-%d %H:%M:%S", tm);

    printf("=== boot-watchos %s ===\n", BUILD);
    printf("time: %s\n", ts);
    printf("pid: %d  uid: %d\n", (int)getpid(), (int)getuid());

    struct utsname u;
    if (uname(&u) == 0) {
        printf("uname: %s %s %s %s\n",
               u.sysname, u.release, u.version, u.machine);
    }

    printf("\n[cpuinfo digest, one-shot]\n");
    dump_file_grep_once("/proc/cpuinfo", "CPU implementer", 1);
    dump_file_grep_once("/proc/cpuinfo", "CPU part", 1);
    dump_file_grep_once("/proc/cpuinfo", "CPU variant", 1);
    dump_file_grep_once("/proc/cpuinfo", "CPU revision", 1);
    dump_file_grep_once("/proc/cpuinfo", "Features", 1);

    printf("\n[sysfs cpu identification]\n");
    dt_prop("/sys/devices/system/cpu/cpu0/regs/identification/midr_el1", 64);
    dt_prop("/sys/devices/system/cpu/cpu0/regs/identification/revidr_el1", 64);

    printf("\n[DT: /sys/firmware/devicetree/base/]\n");
    dt_prop("/sys/firmware/devicetree/base/compatible", 128);
    dt_prop("/sys/firmware/devicetree/base/model", 128);
    dt_prop("/sys/firmware/devicetree/base/cpus/cpu@0/compatible", 128);
    dt_prop("/sys/firmware/devicetree/base/cpus/cpu@0/enable-method", 64);
    /* Reserved memory nodes (ramoops, splash, hyp_region, …) */
    DIR *dir = opendir("/sys/firmware/devicetree/base/reserved-memory");
    if (dir) {
        struct dirent *d;
        while ((d = readdir(dir))) {
            if (d->d_name[0] == '.' || !strchr(d->d_name, '@')) continue;
            char p[256];
            snprintf(p, sizeof p, "/sys/firmware/devicetree/base/reserved-memory/%s/compatible", d->d_name);
            dt_prop(p, 64);
        }
        closedir(dir);
    } else {
        printf("  reserved-memory: opendir errno=%d\n", errno);
    }

    printf("\n[iomem (if readable)]\n");
    dump_file_grep("/proc/iomem", "gic");

    printf("\n[dmesg tail — last ~1 KB of kernel ring]\n");
    int fd = open("/dev/kmsg", O_RDONLY | O_NONBLOCK | O_CLOEXEC);
    if (fd < 0) {
        printf("  /dev/kmsg: errno=%d\n", errno);
    } else {
        /* Seek to end-ish by reading a lot and discarding, then showing last N. */
        char kbuf[16384];
        ssize_t total = 0;
        while (total < (ssize_t)sizeof kbuf) {
            ssize_t r = read(fd, kbuf + total, sizeof kbuf - total);
            if (r <= 0) break;
            total += r;
        }
        close(fd);
        if (total > 0) {
            /* Grep lines containing ID_AA64, isar, mmfr, feature, pac, lse, atomics */
            char *p = kbuf, *end = kbuf + total;
            while (p < end) {
                char *nl = memchr(p, '\n', end - p);
                size_t len = nl ? (size_t)(nl - p) : (size_t)(end - p);
                char line[512];
                size_t clen = len < sizeof line - 1 ? len : sizeof line - 1;
                memcpy(line, p, clen); line[clen] = 0;
                if (strstr(line, "ID_AA64") || strstr(line, "isar") ||
                    strstr(line, "mmfr") || strstr(line, "feature") ||
                    strstr(line, "PAC") || strstr(line, "LSE") ||
                    strstr(line, "atomics") || strstr(line, "detected")) {
                    printf("  %s\n", line);
                }
                p = nl ? nl + 1 : end;
            }
        }
    }

    printf("\n[selinux enforce]\n");
    dump_file("/sys/fs/selinux/enforce", 1);

    printf("\n[write marker]\n");
    char marker_payload[512];
    snprintf(marker_payload, sizeof marker_payload,
             "ts=%s pid=%d uname=%s %s", ts, (int)getpid(), u.sysname, u.release);
    write_marker(marker_payload);

    printf("\n=== done ===\n");
    return 0;
}
