/* sn0w powerkey: root watcher for the POWER key (static ARM32, started by the sn0wbite_powermenu module).
 * Holding POWER for --hold ms (default 2500) opens the sn0wbite power menu. The module turns the stock very-long-press menu off
 * (settings global power_button_very_long_press 0), so this replaces it. The key is read, never grabbed: Android still sees it.
 *   powerkey [--hold MS] [--activity PKG/.CLASS] [--dry] */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

/* start /system/bin/<prog> <args...> (NULL terminated) without waiting for it; SIGCHLD is ignored */
static void run(const char *prog, ...) {
    char path[64];
    const char *av[8];
    int n = 0;
    va_list ap;
    snprintf(path, sizeof path, "/system/bin/%s", prog);
    av[n++] = prog;
    va_start(ap, prog);
    while (n < 7 && (av[n] = va_arg(ap, const char *))) n++;
    va_end(ap);
    av[n] = 0;
    if (fork() == 0) { execv(path, (char *const *)av); _exit(127); }
}

static long now_ms(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return t.tv_sec * 1000L + t.tv_nsec / 1000000L; }

int main(int argc, char **argv) {
    long hold = 2500;
    const char *act = "com.sn0wbite.powermenu/.MenuActivity";
    int dry = 0;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--hold") && i + 1 < argc) hold = atol(argv[++i]);
        else if (!strcmp(argv[i], "--activity") && i + 1 < argc) act = argv[++i];
        else if (!strcmp(argv[i], "--dry")) dry = 1;
    }
    signal(SIGCHLD, SIG_IGN);
    setvbuf(stderr, 0, _IOLBF, 0);
    struct pollfd pf[4];
    int n = 0, wfd = -1;
    for (int i = 0; i < 16 && n < 4; i++) { /* every key device that reports KEY_POWER (the numbering can change) */
        char p[32], name[64] = {0};
        unsigned long keys[(KEY_MAX + 63) / 64] = {0};
        snprintf(p, sizeof p, "/dev/input/event%d", i);
        int fd = open(p, O_RDONLY | O_CLOEXEC);
        if (fd < 0) continue;
        ioctl(fd, EVIOCGNAME(sizeof name - 1), name);
        ioctl(fd, EVIOCGBIT(EV_KEY, sizeof keys), keys);
        if (keys[KEY_POWER / (8 * sizeof(long))] >> (KEY_POWER % (8 * sizeof(long))) & 1) {
            fprintf(stderr, "powerkey: watching %s (%s)\n", p, name);
            pf[n].fd = fd; pf[n].events = POLLIN; n++;
            if (wfd < 0) wfd = open(p, O_WRONLY | O_CLOEXEC); /* to inject the release */
        } else close(fd);
    }
    if (!n) { fprintf(stderr, "powerkey: no device reports KEY_POWER\n"); return 1; }
    /* The OPLUS framework (ShortcutKeyCtrl in services.jar, found with REA) opens the stock power menu with a hard-coded 3000 ms timer started at the
     * press, and a key release cancels that timer. So when our hold time is reached we open our menu and inject the release ourselves, before the
     * stock timer fires. The release also counts as a short press (screen off), so the screen is woken and the menu shown again. */
    long down = -1;
    for (;;) {
        long to = down >= 0 ? down + hold - now_ms() : -1;
        if (down >= 0 && to <= 0) {
            fprintf(stderr, "powerkey: held %ld ms, opening %s and releasing the key%s\n", hold, act, dry ? " (dry)" : "");
            if (!dry) {
                run("am", "start", "-n", act, NULL);
                struct input_event up[2];
                memset(up, 0, sizeof up);
                up[0].type = EV_KEY; up[0].code = KEY_POWER; up[0].value = 0;
                up[1].type = EV_SYN; up[1].code = SYN_REPORT;
                if (write(wfd, up, sizeof up) < 0) fprintf(stderr, "powerkey: inject errno=%d\n", errno);
                usleep(250000);
                run("input", "keyevent", "KEYCODE_WAKEUP", NULL);
                run("am", "start", "-n", act, NULL);
            }
            down = -2; /* wait for the real release, which the kernel ignores after our injected one */
            continue;
        }
        if (poll(pf, n, down >= 0 ? (int)to : -1) <= 0) continue;
        for (int k = 0; k < n; k++) {
            struct input_event ev[8];
            if (!(pf[k].revents & POLLIN)) continue;
            ssize_t r = read(pf[k].fd, ev, sizeof ev);
            for (int j = 0; r > 0 && j < r / (int)sizeof ev[0]; j++)
                if (ev[j].type == EV_KEY && ev[j].code == KEY_POWER) {
                    if (ev[j].value == 1 && down == -1) down = now_ms();
                    else if (ev[j].value == 0) down = -1;
                }
        }
    }
}
