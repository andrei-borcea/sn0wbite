# QEMU T8006 adaptation (work in progress)

`t8006-adaptation.patch` is applied on top of the `qemu-t8030` fork (see [build & boot](../../../../docs/watchos/qemu-build-and-boot.md))
to run our watchOS 6.3.1 (Watch5,4 / T8006) kernelcache on its `t8030` machine. It is the incremental machine-adaptation work
toward kernel console output (phase 2c), not a finished T8006 machine.

So far:
- `apple_a13.c`: make `cluster-id` / `cluster-type` optional in `apple_a13_cpu_create`, so the single-cluster Tempest (S5) CPU node
  from our DeviceTree is accepted (the A13 is big.LITTLE with those props; Tempest has neither). Fixes the first crash.
- `t8030.c`: skip the coprocessor device creates (ANS/SART/SMC/SIO/SPMI/PMU/USB/DART/GPIO/I2C/display) that assert on the T8006
  DeviceTree, for a minimal boot-to-console machine.

Current blocker: skipping the coprocessors removed the `nvram` device (created inside the SMC), which `t8030_memory_setup`
requires. The next step is a proper `hw/arm/t8006.c` that keeps the essentials (CPU, AIC, timer, PMGR, UART, a standalone nvram)
and stubs the rest, rather than skipping by hand. See the chain in [build & boot](../../../../docs/watchos/qemu-build-and-boot.md).

Apply: `cd qemu-t8030 && git apply <this>/t8006-adaptation.patch` then rebuild.
