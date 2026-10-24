# Watch 2 recovery and brick research

Research date: **20 October 2026**, updated **21 October 2026**. Target:
**OPWWE231 A.192**.

**Update, 21 October 2026: the bootloader is now unlocked on our unit, and a
documented, verified restoration path exists.** See
[Root access and boot customization](./root-and-boot-customization.md) for the
full account, including a real recovery performed during this work (a
wrong-architecture Magisk patch produced an unbootable `init_boot`, recovered
through the bootloader's own `EDL mode` / `Reboot` menu, which exposed `adb`
in `recovery` state and let us reach fastboot again). The sections below
describe the **locked-bootloader** state as it stood on 20 October; they are
left as originally written for that record, with this note as the correction.

**Update, 24 October 2026: the watch now runs a custom recovery.** The sections below are about the stock recovery and
brick routes. For the menu we built in the `recovery` partition, and the rules we learned about its size, the BCB and the
display, see [Custom recovery](../custom-boot/custom-recovery.md).

The [fastboot evidence](../../research/opwwe231/2026-10-20-a192/fastboot-excerpts.txt)
records the locked bootloader and unsupported partition readback. The EDL
assessment below comes from public sources, not an executed recovery session.

We have not found a verified public full-restoration procedure for **OPWWE231
A.192**. Recovery options depend on which boot stages still work:

| Device state | Possible route | Evidence and constraints |
| --- | --- | --- |
| Frozen software | Hold Home and Multifunction together for 12 seconds. | The [manufacturer manual](https://fccid.io/2ABZ2-OPWWE231/User-Manual/Users-Manual-7123415.pdf) documents a forced restart. This does not reinstall firmware. |
| Fastboot reachable | Restore compatible original images if the bootloader permits writing. | The current bootloader is locked. We have no validated complete image set or restoration recipe. |
| Android and recovery unavailable | Investigate Qualcomm EDL service recovery. | Device-specific entry, accepted programmer, authentication, firmware compatibility and successful restoration remain unverified. |

The author of the [OPWWE231 porting notes](https://wiki.postmarketos.org/wiki/OnePlus_Watch_2_%28oneplus-opwwe231%29)
reports a brick after flashing a broken recovery, with no ADB or known manual
route back to fastboot. The same author reports `fastboot boot` as unsupported.
This is a historical individual report; current A.192 behavior needs separate
verification.

We excluded apparent successes that concerned a paired phone, `OPWWE234`, or
`OPWWE251`. A public “Watch 2 Firehose” filename also does not establish a usable
programmer: we have not validated the downloaded candidate's format, hardware
compatibility or acceptance by the watch.

A factory reset erases user data without reinstalling corrupted boot partitions.
If local recovery proves unavailable, [OnePlus support](https://www.oneplus.com/it/support/repair)
must assess service eligibility and repair options. The repair page warns that
service erases data.

## Evidence needed before relying on EDL

- A repeatable entry and exit procedure for this model when Android cannot boot.
- An inspectable service package with compatible project, board, MCU and build metadata.
- A programmer that the device accepts, with any required read and write authorization.
- Applicable rollback rules and a documented successful restoration on this model.

The service listing's `OWWE231` name alone does not prove or exclude `OPWWE231`
compatibility. Its generic phone volume-button instructions do not establish
an entry procedure on the watch. Newer-model EDL reports require separate
validation before applying them to this device.

A successful readback would satisfy only part of the [backup requirements](./device-and-storage.md).
Neither the current diagnostics nor an older stock package preserves the owner's
complete device state.

## Cross-reference: OnePlus Watch 3 EDL research (same chip, same wall)

A public repository researching EDL recovery for the **OnePlus Watch 3 (OPWWE251)**
independently confirms the Watch 3 shares our Watch 2's Monaco/SW5100 platform, and
its own detailed findings — not its summary table — line up with everything above:

- **No partition read or write was achieved.** Its own `edl_confirmation.md` lists
  "reading partitions via EDL" as unknown, pending a loader that was never obtained.
  The README's "EDL read: works" line describes reaching EDL mode, not reading data.
- **EDL entry on the Watch 3 needed a physical D+/GND short** on the board. Our Watch 2
  needed no hardware at all — `adb reboot edl` drops it into Sahara mode from software
  alone (see the EDL finding already in this document's history).
- Qualcomm's VIP (Validated Image Programming) protocol gates any real flash/read
  operation behind authorization packets generated live by OPPO's own signing backend
  (WCSM/OCSM) and discarded after each session. The researcher reconstructed the rest of
  the flash protocol but explicitly could not substitute for these two packets, and
  named obtaining them from someone with backend access as their own next step — i.e.,
  insider access to OPPO's infrastructure, not a reverse-engineering gap.
- The same source independently confirms CVE-2026-25262 (the Qualcomm Sahara
  write-what-where disclosed at Black Hat Asia 2026) does not list Monaco among
  affected chips, matching our own reading of the advisory.

Net effect: an independent party investigating the same chip generation hit the same
wall for the same reason — a live, server-side cryptographic gate designed specifically
to prevent exactly this kind of offline EDL write — strengthening rather than changing
the conclusion above. EDL is not currently a usable restoration or extraction path for
this device without OEM backend cooperation.
