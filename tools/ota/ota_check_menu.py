#!/usr/bin/env python3
"""Interactive helper for the reconstructed OPPO/HeyTap OTA "Query_Update" check.

Lets you pick a connected ADB device, reads its build-version properties with
`adb shell getprop` (no root needed), and runs the real `Query_Update` request
against the vendor server using `oppo_ota_query.py` in this same directory.

This only ever sends the device's own, real, current version strings. It does
not offer an option to declare a false/older version to the vendor server —
that is a different, more invasive action and is left out on purpose; see
docs/opwwe231/ota-and-ohealth.md for why, and oppo_ota_query.py's own --sweep mode
for how to run that step yourself if you choose to.

Usage:
    python3 ota_check_menu.py
"""
import json
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from oppo_ota_query import HOSTS, decrypt_resps, encrypt_body  # noqa: E402

PROPS = {
    "ota_version": "ro.build.version.ota",
    "model": "ro.product.model",
    "regionmark": "ro.vendor.oplus.regionmark",
    "fingerprint": "ro.build.fingerprint",
}

REGION_GUESS = {"EU": "eu", "IN": "in", "US": "us", "NA": "us"}


def run(cmd: list[str]) -> str:
    return subprocess.run(cmd, capture_output=True, text=True, check=True).stdout.strip()


def list_devices() -> list[dict]:
    try:
        out = run(["adb", "devices", "-l"])
    except (FileNotFoundError, subprocess.CalledProcessError):
        print("adb not found or failed to run. Is it installed and on PATH?")
        sys.exit(1)
    devices = []
    for line in out.splitlines()[1:]:
        line = line.strip()
        if not line or line.startswith("*"):
            continue
        parts = line.split()
        serial, state = parts[0], parts[1] if len(parts) > 1 else "?"
        model = next((p.split(":", 1)[1] for p in parts if p.startswith("model:")), "?")
        product = next((p.split(":", 1)[1] for p in parts if p.startswith("product:")), "?")
        devices.append({"serial": serial, "state": state, "model": model, "product": product})
    return devices


def pick_device(devices: list[dict]) -> dict:
    print("\nConnected ADB devices:")
    for i, d in enumerate(devices, 1):
        print(f"  [{i}] {d['serial']}  model={d['model']}  product={d['product']}  state={d['state']}")
    while True:
        choice = input("Pick a device by number: ").strip()
        if choice.isdigit() and 1 <= int(choice) <= len(devices):
            return devices[int(choice) - 1]
        print("Invalid choice, try again.")


def getprop(serial: str, prop: str) -> str:
    try:
        return run(["adb", "-s", serial, "shell", "getprop", prop])
    except subprocess.CalledProcessError:
        return ""


def detect_versions(serial: str) -> dict:
    values = {k: getprop(serial, p) for k, p in PROPS.items()}
    rom_match = re.search(r"A\.\d+", values["ota_version"])
    values["rom_version"] = rom_match.group(0) if rom_match else ""
    values["region"] = next((v for k, v in REGION_GUESS.items()
                              if values["regionmark"].upper().startswith(k)), "eu")
    return values


def confirm_or_edit(label: str, default: str) -> str:
    entered = input(f"{label} [{default}]: ").strip()
    return entered or default


def derive_ota_version(template: str, rom_version: str) -> str:
    """Swap the A.<n> token inside a real otaVersion string for a different
    romVersion, keeping the rest of the format (build counter, timestamp) as
    the real current build's. This is only a starting point: the build
    counter and timestamp belong to a specific build we have no record of
    for any romVersion other than the one actually installed, so the result
    is not guaranteed to be a real otaVersion string for that romVersion."""
    token = rom_version if re.fullmatch(r"A\.\d+", rom_version) else f"A.{rom_version.lstrip('A.')}"
    new, n = re.subn(r"A\.\d+", token, template, count=1)
    return new if n else template


def do_query(product: str, ota_version: str, rom_version: str, region: str):
    plain = {
        "version": "3",
        "otaPrefix": product,
        "productName": product,
        "otaVersion": ota_version,
        "romVersion": rom_version,
        "imei": "000000000000000",
        "mode": 0,
        "language": "en-US",
        "time": __import__("time").time_ns() // 1_000_000,
        "androidVersion": "unknown",
        "colorOSVersion": "unknown",
        "registrationId": "unknown",
        "type": "0",
        "modules": [{"otaPkgType": "1", "version": ota_version}],
    }
    body = encrypt_body(plain)
    url = HOSTS[region] + "Query_Update"
    print(f"\nPOST {url}")
    print("request (plaintext before encryption):", json.dumps(plain))

    import urllib.error
    import urllib.request
    req = urllib.request.Request(
        url, data=json.dumps(body).encode(),
        headers={"Content-Type": "application/json; charset=UTF-8"}, method="POST")
    try:
        with urllib.request.urlopen(req, timeout=30) as r:
            raw, status = r.read().decode(), r.status
    except urllib.error.HTTPError as e:
        raw, status = e.read().decode(errors="replace"), e.code
    except urllib.error.URLError as e:
        print(f"Network error: {e}")
        return

    print(f"HTTP {status}")
    try:
        outer = json.loads(raw)
    except ValueError:
        print("non-JSON response:", raw[:500])
        return
    if outer.get("resps"):
        print("\nDecrypted result:")
        print(json.dumps(decrypt_resps(outer["resps"]), indent=2))
    else:
        print(json.dumps(outer, indent=2))


def main():
    devices = list_devices()
    if not devices:
        print("No ADB devices found (adb devices -l returned none).")
        sys.exit(1)
    device = pick_device(devices)
    serial = device["serial"]

    print(f"\nReading build properties from {serial} ...")
    detected = detect_versions(serial)
    print(f"  ro.build.version.ota = {detected['ota_version']}")
    print(f"  ro.product.model     = {detected['model']}")
    print(f"  ro.vendor.oplus.regionmark = {detected['regionmark']}")
    print(f"  inferred romVersion  = {detected['rom_version']}")
    print(f"  inferred region      = {detected['region']}")

    while True:
        print("\n--- OTA Query_Update menu ---")
        print("  [1] Show detected device properties again")
        print("  [2] Run real Query_Update check with the device's own current version")
        print("  [3] Decrypt a pasted 'resps' string")
        print("  [4] Re-select ADB device")
        print("  [0] Exit")
        choice = input("> ").strip()

        if choice == "1":
            for k, v in detected.items():
                print(f"  {k}: {v}")

        elif choice == "2":
            product = confirm_or_edit("Product name", detected["model"])
            rom_version = confirm_or_edit("romVersion", detected["rom_version"])
            suggested_ota = derive_ota_version(detected["ota_version"], rom_version)
            if rom_version != detected["rom_version"]:
                print(f"  note: otaVersion below has the A.xxx token swapped to match "
                      f"romVersion={rom_version}, but its build counter and timestamp "
                      f"are still the real current build's ({detected['ota_version']}). "
                      "This is a guess, not a confirmed otaVersion string for that build.")
            ota_version = confirm_or_edit("otaVersion (derived from romVersion, edit if you "
                                           "know the real build counter/timestamp)",
                                           suggested_ota)
            region = confirm_or_edit("Region (eu/us/in/default)", detected["region"])
            if region not in HOSTS:
                print(f"Unknown region '{region}', defaulting to eu.")
                region = "eu"
            do_query(product, ota_version, rom_version, region)

        elif choice == "3":
            resps = input("Paste the 'resps' value: ").strip()
            try:
                print(json.dumps(decrypt_resps(resps), indent=2))
            except Exception as e:
                print(f"Could not decrypt: {e}")

        elif choice == "4":
            devices = list_devices()
            if not devices:
                print("No ADB devices found.")
                continue
            device = pick_device(devices)
            serial = device["serial"]
            detected = detect_versions(serial)

        elif choice == "0":
            break

        else:
            print("Invalid choice.")


if __name__ == "__main__":
    main()
