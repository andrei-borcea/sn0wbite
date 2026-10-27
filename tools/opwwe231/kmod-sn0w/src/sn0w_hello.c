/* sn0w_hello: minimal "I loaded" kernel module for the OPW2 Wear OS kernel.
 *
 * First iteration — proves the toolchain + vermagic + CRC pipeline end-to-end.
 * Does nothing beyond printing to the kernel log on load/unload. Once insmod
 * accepts this .ko, we extend it (contiguous memory, smp_send_stop, custom
 * cpu_soft_restart asm, boot_args + DT setup, watchOS kernelcache jump).
 *
 * Target: 5.15.185-android14-11-maybe-dirty aarch64 (Qualcomm W5 Gen 1 Kryo).
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

static int __init sn0w_init(void)
{
    pr_info("sn0w: hello from the OPW2 kernel; module loaded at %p\n", sn0w_init);
    return 0;
}

static void __exit sn0w_exit(void)
{
    pr_info("sn0w: goodbye; module unloading\n");
}

module_init(sn0w_init);
module_exit(sn0w_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("sn0wbite");
MODULE_DESCRIPTION("sn0wbite OPW2 watchOS handoff — stage 0 (hello)");
MODULE_VERSION("0.0.1");
