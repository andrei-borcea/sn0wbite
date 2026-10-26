// sn0shell — a tiny REPL for Watch7,11 (watchOS 27) under QEMU-SPTM.
// stdin/stdout/stderr are wired to /dev/console by the LaunchDaemon plist, so
// read(0) reaches whatever QEMU's `-serial mon:stdio` is receiving. Keep this
// program self-contained: libSystem is the only linked dylib, no CRT runtime
// beyond what libc provides.

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/host_info.h>
#include <mach/vm_statistics.h>
#include <mach/processor_info.h>

// watchOS SDK marks the mach host APIs as unavailable, but libSystem exports
// the symbols and the kernel implements the MIG endpoints. Alias the real
// symbols under local names so the availability attribute doesn't trip.
extern kern_return_t sn_host_info(host_t, host_flavor_t, host_info_t,
                                  mach_msg_type_number_t *)      __asm("_host_info");
extern kern_return_t sn_host_statistics64(host_t, host_flavor_t, host_info64_t,
                                          mach_msg_type_number_t *) __asm("_host_statistics64");
extern kern_return_t sn_host_processor_info(host_t, processor_flavor_t,
                                            natural_t *,
                                            processor_info_array_t *,
                                            mach_msg_type_number_t *) __asm("_host_processor_info");
extern kern_return_t sn_host_kernel_version(host_t, kernel_version_t) __asm("_host_kernel_version");

extern int __getdirentries64(int fd, void *buf, size_t bufsize, long *basep);

#define LINE_MAX_LEN 256
#define BANNER "\r\n" \
    "+============================================================+\r\n" \
    "|  sn0shell  --  Watch7,11 / watchOS 27 / QEMU-SPTM userspace |\r\n" \
    "|  type `help' for commands, `exit' to leave the REPL         |\r\n" \
    "+============================================================+\r\n"
#define PROMPT "sn0shell# "

static void w(int fd, const char *s) {
    write(fd, s, strlen(s));
}

static void putnum(int fd, long long n) {
    char buf[32];
    int neg = 0, i = sizeof(buf);
    if (n < 0) { neg = 1; n = -n; }
    if (n == 0) { buf[--i] = '0'; }
    while (n > 0) { buf[--i] = '0' + (n % 10); n /= 10; }
    if (neg) { buf[--i] = '-'; }
    write(fd, buf + i, sizeof(buf) - i);
}

// Line-by-line read from fd 0. Handles LF, CR and CRLF. Echoes to fd 1 so the
// typist sees what they're entering (console is not in icanon mode here).
static ssize_t read_line(char *buf, size_t cap) {
    size_t len = 0;
    while (len + 1 < cap) {
        char ch;
        ssize_t n = read(0, &ch, 1);
        if (n <= 0) return n;
        if (ch == '\r' || ch == '\n') {
            w(1, "\r\n");
            break;
        }
        if (ch == 0x7f || ch == 0x08) {
            if (len > 0) { len--; w(1, "\b \b"); }
            continue;
        }
        if (ch < 0x20) continue;
        buf[len++] = ch;
        write(1, &ch, 1);
    }
    buf[len] = 0;
    return (ssize_t)len;
}

static int streq(const char *a, const char *b) { return strcmp(a, b) == 0; }
static int startswith(const char *s, const char *p) {
    size_t pl = strlen(p);
    return strncmp(s, p, pl) == 0 && (s[pl] == ' ' || s[pl] == 0);
}

static void cmd_help(void) {
    w(1,
      "commands:\r\n"
      "  help              show this message\r\n"
      "  version           print sn0shell + kernel version\r\n"
      "  echo <text>       echo <text> to the console\r\n"
      "  cat <path>        print a file (first 4 KiB)\r\n"
      "  ls <path>         read-dir via getdirentries64\r\n"
      "  sysctl <name>     query sysctl by name (returns string/int)\r\n"
      "  uptime            seconds since boot\r\n"
      "  write <path> <s>  write <s> to <path> (overwrites)\r\n"
      "  pid               print our pid\r\n"
      "  mach              mach host info (cpus, memory, kernel version)\r\n"
      "  cpuload           per-cpu tick counters via processor_info\r\n"
      "  vmstat            host vm statistics\r\n"
      "  devices           list /dev entries + a few diagnostic sysctls\r\n"
      "  exit              return from main → launchd will respawn us\r\n");
}

static void cmd_mach(void) {
    host_t host = mach_host_self();
    host_basic_info_data_t basic;
    mach_msg_type_number_t count = HOST_BASIC_INFO_COUNT;
    if (sn_host_info(host, HOST_BASIC_INFO, (host_info_t)&basic, &count) == KERN_SUCCESS) {
        w(1, "max_cpus:        "); putnum(1, basic.max_cpus); w(1, "\r\n");
        w(1, "avail_cpus:      "); putnum(1, basic.avail_cpus); w(1, "\r\n");
        w(1, "physical_cpu:    "); putnum(1, basic.physical_cpu); w(1, "\r\n");
        w(1, "logical_cpu:     "); putnum(1, basic.logical_cpu); w(1, "\r\n");
        w(1, "memory_size:     "); putnum(1, (long long)basic.max_mem); w(1, " bytes\r\n");
        w(1, "cpu_type:        "); putnum(1, basic.cpu_type); w(1, " (0x100000C = arm64/arm64e)\r\n");
        w(1, "cpu_subtype:     "); putnum(1, basic.cpu_subtype); w(1, "\r\n");
    } else {
        w(2, "host_info HOST_BASIC_INFO failed\r\n");
    }

    kernel_version_t kv_buf;
    if (sn_host_kernel_version(host, kv_buf) == KERN_SUCCESS) {
        w(1, "kernel_version:  "); w(1, kv_buf); w(1, "\r\n");
    }
}

static void cmd_cpuload(void) {
    host_t host = mach_host_self();
    processor_info_array_t infoArr;
    mach_msg_type_number_t infoCount;
    natural_t numCpus;
    kern_return_t kr = sn_host_processor_info(host, PROCESSOR_CPU_LOAD_INFO,
                                              &numCpus, &infoArr, &infoCount);
    if (kr != KERN_SUCCESS) {
        w(2, "host_processor_info failed: "); putnum(2, kr); w(2, "\r\n"); return;
    }
    processor_cpu_load_info_t loads = (processor_cpu_load_info_t)infoArr;
    for (natural_t i = 0; i < numCpus; i++) {
        w(1, "cpu["); putnum(1, i); w(1, "] user=");
        putnum(1, loads[i].cpu_ticks[CPU_STATE_USER]);
        w(1, " sys=");    putnum(1, loads[i].cpu_ticks[CPU_STATE_SYSTEM]);
        w(1, " idle=");   putnum(1, loads[i].cpu_ticks[CPU_STATE_IDLE]);
        w(1, " nice=");   putnum(1, loads[i].cpu_ticks[CPU_STATE_NICE]);
        w(1, "\r\n");
    }
    vm_deallocate(mach_task_self(), (vm_address_t)infoArr, infoCount * sizeof(int));
}

static void cmd_vmstat(void) {
    host_t host = mach_host_self();
    vm_statistics64_data_t vm;
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    if (sn_host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&vm, &count) != KERN_SUCCESS) {
        w(2, "host_statistics64 failed\r\n"); return;
    }
    w(1, "free_count:     "); putnum(1, (long long)vm.free_count); w(1, "\r\n");
    w(1, "active_count:   "); putnum(1, (long long)vm.active_count); w(1, "\r\n");
    w(1, "inactive_count: "); putnum(1, (long long)vm.inactive_count); w(1, "\r\n");
    w(1, "wire_count:     "); putnum(1, (long long)vm.wire_count); w(1, "\r\n");
    w(1, "faults:         "); putnum(1, (long long)vm.faults); w(1, "\r\n");
    w(1, "cow_faults:     "); putnum(1, (long long)vm.cow_faults); w(1, "\r\n");
    w(1, "pageins:        "); putnum(1, (long long)vm.pageins); w(1, "\r\n");
    w(1, "pageouts:       "); putnum(1, (long long)vm.pageouts); w(1, "\r\n");
}

static void cmd_devices(void) {
    w(1, "-- /dev --\r\n");
    int fd = open("/dev", O_RDONLY | O_DIRECTORY);
    if (fd < 0) { w(2, "/dev open failed\r\n"); goto diag; }
    char buf[4096];
    long bp = 0;
    ssize_t n = __getdirentries64(fd, buf, sizeof(buf), &bp);
    close(fd);
    if (n > 0) {
        char *p = buf, *end = buf + n;
        while (p < end) {
            uint16_t reclen = *(uint16_t *)(p + 16);
            uint16_t namlen = *(uint16_t *)(p + 18);
            const char *name = p + 21;
            if (reclen == 0) break;
            write(1, name, namlen); w(1, "  ");
            p += reclen;
        }
        w(1, "\r\n");
    }
diag:;
    static const char *mibs[] = {
        "hw.machine", "hw.model", "hw.ncpu", "hw.memsize",
        "machdep.cpu.brand_string", "kern.osrelease", "kern.osversion",
        "kern.boottime", NULL,
    };
    w(1, "-- diag sysctls --\r\n");
    for (int i = 0; mibs[i]; i++) {
        char val[256]; size_t sz = sizeof(val);
        if (sysctlbyname(mibs[i], val, &sz, NULL, 0) == 0) {
            w(1, "  "); w(1, mibs[i]); w(1, " = ");
            if (sz == sizeof(int)) { putnum(1, *(int *)val); }
            else if (sz == sizeof(long long)) { putnum(1, *(long long *)val); }
            else { write(1, val, sz); }
            w(1, "\r\n");
        }
    }
}

static void cmd_version(void) {
    w(1, "sn0shell 0.2 (sn0wbite / Watch7,11 / watchOS 27)\r\n");
    char buf[256];
    size_t sz = sizeof(buf);
    if (sysctlbyname("kern.version", buf, &sz, NULL, 0) == 0) {
        w(1, "kern.version: "); write(1, buf, sz); w(1, "\r\n");
    }
    sz = sizeof(buf);
    if (sysctlbyname("hw.machine", buf, &sz, NULL, 0) == 0) {
        w(1, "hw.machine:   "); write(1, buf, sz); w(1, "\r\n");
    }
    sz = sizeof(buf);
    if (sysctlbyname("hw.model", buf, &sz, NULL, 0) == 0) {
        w(1, "hw.model:     "); write(1, buf, sz); w(1, "\r\n");
    }
}

static void cmd_cat(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) { w(2, "open: "); w(2, strerror(errno)); w(2, "\r\n"); return; }
    char buf[1024];
    ssize_t n, total = 0;
    while ((n = read(fd, buf, sizeof(buf))) > 0 && total < 4096) {
        write(1, buf, n);
        total += n;
    }
    close(fd);
    w(1, "\r\n");
    w(1, "-- "); putnum(1, total); w(1, " bytes --\r\n");
}

static void cmd_write(const char *rest) {
    const char *sp = strchr(rest, ' ');
    if (!sp) { w(2, "usage: write <path> <text>\r\n"); return; }
    char path[256];
    size_t pl = sp - rest;
    if (pl >= sizeof(path)) { w(2, "path too long\r\n"); return; }
    memcpy(path, rest, pl); path[pl] = 0;
    const char *text = sp + 1;
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { w(2, "open: "); w(2, strerror(errno)); w(2, "\r\n"); return; }
    ssize_t n = write(fd, text, strlen(text));
    close(fd);
    w(1, "wrote "); putnum(1, n); w(1, " bytes\r\n");
}

static void cmd_sysctl(const char *name) {
    char buf[1024];
    size_t sz = sizeof(buf);
    if (sysctlbyname(name, buf, &sz, NULL, 0) < 0) {
        w(2, "sysctl: "); w(2, strerror(errno)); w(2, "\r\n");
        return;
    }
    if (sz == sizeof(int)) {
        putnum(1, *(int *)buf); w(1, "\r\n");
    } else if (sz == sizeof(long long)) {
        putnum(1, *(long long *)buf); w(1, "\r\n");
    } else {
        write(1, buf, sz);
        if (sz == 0 || buf[sz - 1] != '\n') w(1, "\r\n");
    }
}

static void cmd_uptime(void) {
    struct timeval boot;
    size_t sz = sizeof(boot);
    int mib[2] = {CTL_KERN, KERN_BOOTTIME};
    if (sysctl(mib, 2, &boot, &sz, NULL, 0) < 0) {
        w(2, "sysctl boottime: "); w(2, strerror(errno)); w(2, "\r\n"); return;
    }
    struct timeval now;
    gettimeofday(&now, NULL);
    long secs = now.tv_sec - boot.tv_sec;
    putnum(1, secs); w(1, " s since boot\r\n");
}

static void cmd_ls(const char *path) {
    int fd = open(path, O_RDONLY | O_DIRECTORY);
    if (fd < 0) { w(2, "open: "); w(2, strerror(errno)); w(2, "\r\n"); return; }
    char buf[4096];
    long bp = 0;
    ssize_t n = __getdirentries64(fd, buf, sizeof(buf), &bp);
    close(fd);
    if (n < 0) { w(2, "getdirentries64 failed\r\n"); return; }
    char *p = buf;
    char *end = buf + n;
    int count = 0;
    while (p < end) {
        // dirent64: u64 d_ino, u64 d_seekoff, u16 d_reclen, u16 d_namlen, u8 d_type, char d_name[]
        uint16_t reclen = *(uint16_t *)(p + 16);
        uint16_t namlen = *(uint16_t *)(p + 18);
        const char *name = p + 21;
        if (reclen == 0) break;
        write(1, name, namlen); w(1, "\r\n");
        p += reclen;
        count++;
    }
    w(1, "-- "); putnum(1, count); w(1, " entries --\r\n");
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    w(1, BANNER);
    char line[LINE_MAX_LEN];
    for (;;) {
        w(1, PROMPT);
        ssize_t n = read_line(line, sizeof(line));
        if (n < 0) {
            w(2, "read error, exiting\r\n");
            return 1;
        }
        if (n == 0) continue;
        if (streq(line, "exit") || streq(line, "quit")) {
            w(1, "bye.\r\n");
            return 0;
        }
        if (streq(line, "help") || streq(line, "?")) { cmd_help(); continue; }
        if (streq(line, "version"))                   { cmd_version(); continue; }
        if (streq(line, "uptime"))                    { cmd_uptime(); continue; }
        if (streq(line, "pid")) {
            putnum(1, (long long)getpid()); w(1, "\r\n"); continue;
        }
        if (streq(line, "mach"))        { cmd_mach(); continue; }
        if (streq(line, "cpuload"))     { cmd_cpuload(); continue; }
        if (streq(line, "vmstat"))      { cmd_vmstat(); continue; }
        if (streq(line, "devices"))     { cmd_devices(); continue; }
        if (startswith(line, "echo"))   { w(1, line + 5); w(1, "\r\n"); continue; }
        if (startswith(line, "cat"))    { cmd_cat(line + 4); continue; }
        if (startswith(line, "ls"))     { cmd_ls(line + 3); continue; }
        if (startswith(line, "sysctl")) { cmd_sysctl(line + 7); continue; }
        if (startswith(line, "write"))  { cmd_write(line + 6); continue; }
        w(2, "unknown: "); w(2, line); w(2, "   (try `help')\r\n");
    }
}
