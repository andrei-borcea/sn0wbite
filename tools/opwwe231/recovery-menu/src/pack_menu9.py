#!/usr/bin/env python3
"""Build the sn0wbite boot menu recovery image (v9) from the stock one, inside the stock image size.

 - adds   system/bin/sn0wmenu             the menu and its watchdog (static ARM32); the stock system/bin/recovery is NOT touched
 - adds   system/etc/sn0w-targets.conf    the DUAL BOOT list
 - edits  system/etc/init/hw/init.rc      service "recovery" now runs sn0wmenu and is oneshot (never restarted)
 - edits  init.recovery.qcom.rc           service sn0w_deadman (init supervised): when it exits, init reboots
 - drops  cosmetic/unneeded files to make room (installing-animation frames beyond the first 3, erofs tools)
Hard limit: the stock partition keeps AVB metadata right after the image (footer original_image_size = 12083200).
An image larger than that overwrites it and the bootloader refuses recovery (observed 2026-10-23, v2).
Usage: python3 -I pack_menu9.py <stock recovery.img> <sn0wmenu binary> <sn0w-targets.conf> <out img>
"""
import gzip, re, struct, sys

LIMIT = 12083200
MARGIN = 65536
RC_SNIPPET = b"""
# ---- sn0wbite boot menu (v9) ----
# Kill switch: sn0wmenu --deadman watches the menu's heartbeat, its idle deadline and the battery. When it exits, init (which owns the
# reboot permission) reboots. The menu pins it (heartbeat file) while the stock recovery runs.
service sn0w_deadman /system/bin/sn0wmenu --deadman
    seclabel u:r:recovery:s0
    user root
    oneshot

on init
    setprop sn0w.armed 0
    start sn0w_deadman
    setprop sn0w.armed 1

on property:init.svc.sn0w_deadman=stopped && property:sn0w.armed=1
    setprop sys.powerctl reboot
"""
DROP = [re.compile(r"^res/images/loop(\d{5})\.png$"), re.compile(r"^system/bin/(mkfs|fsck|dump)\.erofs$")]

stock, newbin, conf, out = sys.argv[1:5]
img = open(stock, "rb").read()
hdr = bytearray(img[:4096])
assert hdr[:8] == b"ANDROID!"
ver = struct.unpack_from("<I", hdr, 40)[0]
ksz, rsz = struct.unpack_from("<II", hdr, 8)
assert ver == 4 and ksz == 0 and img[4096:4098] == b"\x1f\x8b"
raw = gzip.decompress(img[4096:4096 + rsz])


def parse(buf):
    off, ents = 0, []
    while True:
        assert buf[off:off + 6] == b"070701", f"bad cpio magic at {off}"
        f = [int(buf[off + 6 + 8 * i: off + 14 + 8 * i], 16) for i in range(13)]
        ns, fs = f[11], f[6]
        name = buf[off + 110: off + 110 + ns - 1].decode()
        d0 = (off + 110 + ns + 3) & ~3
        ents.append([f, name, buf[d0: d0 + fs]])
        off = (d0 + fs + 3) & ~3
        if name == "TRAILER!!!":
            return ents


def build(ents):
    o = bytearray()
    for f, name, data in ents:
        f = list(f)
        f[6], f[11] = len(data), len(name) + 1
        o += b"070701" + b"".join(b"%08X" % v for v in f) + name.encode() + b"\0"
        o += b"\0" * (-len(o) % 4) + data
        o += b"\0" * (-len(o) % 4)
    o += b"\0" * (-len(o) % 512)
    return bytes(o)


ents = parse(raw)
assert all(e[1] not in ("system/bin/sn0wmenu", "system/bin/sn0wprobe") for e in ents), "image already modified"
orig = {e[1]: bytes(e[2]) for e in ents}

# 1. drop files (never one that is part of a hardlink group)
kept, dropped = [], []
for e in ents:
    n = e[1]
    m = DROP[0].match(n)
    drop = (m and int(m.group(1)) >= 3) or DROP[1].match(n)
    if drop:
        assert e[0][4] == 1, f"{n} has nlink {e[0][4]}: refusing to drop a hardlinked file"
        dropped.append((n, len(e[2])))
    else:
        kept.append(e)
ents = kept

# 2. add the menu next to the stock recovery binary (copy its header so mode/uid/gid match) and the targets file (init.rc's header)
i = [k for k, e in enumerate(ents) if e[1] == "system/bin/recovery"]
assert len(i) == 1
base = ents[i[0]]
rcent = [e for e in ents if e[1] == "system/etc/init/hw/init.rc"]
assert len(rcent) == 1
new_ino = max(e[0][0] for e in ents) + 1
menu = open(newbin, "rb").read()
ents.insert(i[0] + 1, [[new_ino] + base[0][1:], "system/bin/sn0wmenu", menu])
# The kernel's initramfs unpacker does not create parent directories: a file listed before its directory is silently dropped
# (v9 lost sn0w-targets.conf that way). init.rc sits in system/etc/init/hw, so system/etc exists by the time we get there.
k = [n for n, e in enumerate(ents) if e[1] == "system/etc/init/hw/init.rc"][0]
ents.insert(k + 1, [[new_ino + 1] + rcent[0][0][1:], "system/etc/sn0w-targets.conf", open(conf, "rb").read()])

# 3. the recovery service runs the menu, oneshot
old = b"service recovery /system/bin/recovery\n"
assert rcent[0][2].count(old) == 1, "service recovery line not found exactly once"
rcent[0][2] = rcent[0][2].replace(old, b"service recovery /system/bin/sn0wmenu\n    oneshot\n")

# 4. deadman rc
q = [e for e in ents if e[1] == "init.recovery.qcom.rc"]
assert len(q) == 1
q[0][2] = q[0][2].rstrip(b"\n") + b"\n" + RC_SNIPPET

# 5. nothing else may differ from the stock ramdisk
changed = sorted(e[1] for e in ents if e[1] in orig and bytes(e[2]) != orig[e[1]])
added = sorted(e[1] for e in ents if e[1] not in orig)
assert changed == ["init.recovery.qcom.rc", "system/etc/init/hw/init.rc"], changed
assert added == ["system/bin/sn0wmenu", "system/etc/sn0w-targets.conf"], added
assert next(e for e in ents if e[1] == "system/bin/recovery")[2] == orig["system/bin/recovery"]

seen = set()
for _, name, _ in ents:
    parent = name.rsplit("/", 1)[0] if "/" in name else ""
    assert not parent or parent in seen, f"{name}: parent directory {parent} is not listed before it, the kernel would drop the file"
    seen.add(name)

cpio = build(ents)
rd = gzip.compress(cpio, 9, mtime=0)
struct.pack_into("<I", hdr, 12, len(rd))
body = bytes(hdr) + rd
body += b"\0" * (-len(body) % 4096)
print(f"changed {changed}\nadded {added}\ndropped {len(dropped)} files, {sum(s for _, s in dropped)} B raw; menu {len(menu)} B")
print(f"image {len(body)} B, limit {LIMIT}, headroom {LIMIT - len(body)} B (ramdisk gz {len(rd)}, entries {len(ents)})")
if len(body) > LIMIT - MARGIN:
    sys.exit(f"REFUSED: image exceeds the safe size ({LIMIT - MARGIN} B); it would overwrite the AVB metadata")
open(out, "wb").write(body)
print("wrote", out)
