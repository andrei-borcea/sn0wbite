# sn0wbite 🍎❄️

> *One bite is all it takes.*

**sn0wbite** is an experimental reverse engineering project aiming to run
watchOS natively on the **OnePlus Watch 2R**.

The goal is to understand the watchOS boot process and its dependencies on Apple
hardware, then investigate the changes needed to boot it on the Watch 2R.
The first watchOS boot milestone is repeatable execution of the watchOS kernel
on the device, with logs or other traces that show how far execution gets.
A usable watchOS interface and working peripherals are later milestones.

## Status

🚧 **Early research. Feasibility remains unproven.**

The repository currently contains this README. There is no implemented loader
or demonstrated watchOS boot on the Watch 2R. The roadmap below describes planned
work; it does not represent validated capabilities.

## Target

| Component | Research target |
| --- | --- |
| Device | OnePlus Watch 2R |
| Main platform | Qualcomm Snapdragon W5 |
| Companion processor | BES2700 |
| Operating system to boot | Apple watchOS |
| Reference Apple Watch model and watchOS version | To be selected |

The Watch 2R firmware version, hardware revision, and reference watchOS build
must be recorded before drawing conclusions about compatibility.

## Technical challenges

A shared ARM instruction set does not establish compatibility. Research needs
to cover firmware verification, the boot handoff, kernel platform assumptions,
and dependencies on Apple hardware and services.

On the Watch 2R, the work includes understanding memory layout, interrupt and
timer setup, storage, and peripheral interfaces. Display, touch, buttons, sensors,
and communication with the companion processor need hardware support before
the device can become usable.

## Roadmap

- [ ] **Establish the device baseline.** Record hardware and stock firmware
  details, map partitions and boot stages, and investigate backup and recovery.
- [ ] **Analyze watchOS.** Select a reference device and firmware version;
  document the boot chain, kernel format, and hardware dependencies.
- [ ] **Assess feasibility.** Identify a viable execution path on the Watch 2R
  and document verification constraints and unresolved blockers.
- [ ] **Run a minimal payload.** Establish controlled code execution and a way
  to capture diagnostic output before attempting a watchOS boot.
- [ ] **Attempt watchOS kernel bring-up.** Investigate the loader, boot data,
  and platform support needed for the kernel to execute on the target hardware.
- [ ] **Demonstrate a repeatable boot.** Publish reproduction steps and traces
  that identify the watchOS build and the execution stage reached.
- [ ] **Bring up the interface and peripherals.** Work toward a first visible
  watchOS screen, input, sensors, and power management as kernel progress allows.

## Research notes

For each finding, record the device and firmware versions, tools, steps, and
supporting evidence. Separate confirmed observations from hypotheses. For boot
attempts, include diagnostic output and the last confirmed execution stage.
Include firmware SHA-256 hashes and the source revision of any payload or loader
used in the attempt.

## Disclaimer

This project is for educational and research purposes only.
Not affiliated with, endorsed by, or connected to Apple Inc. or OnePlus.
watchOS is a trademark of Apple Inc.

Modifying or flashing firmware can erase data or brick the device.
Use at your own risk.
