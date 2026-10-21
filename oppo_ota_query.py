#!/usr/bin/env python3
"""Client for the OPPO/HeyTap OTA "Query_Update" API as implemented by OHealth 4.62.7.

Reconstructed from static analysis of com.heytap.health.device.ota.cloud.OppoOtaUtils,
rf.a (AES-CTR helper) and com.heytap.health.device.ota.check.e0.f (request body),
during reverse engineering of the paired-phone OHealth app for OPWWE231 / A.192.
Re-verified on 20 October 2026 with a jadx decompile (jadx-headless-mcp 0.7.1)
after the first pass had used smali only; see the field notes below for what
that pass corrected.

Verified against the live server on 20 October 2026: the encryption/decryption
round-trip matches the real ifota-term-eu.coloros.com responses, and the real
OHealth app's own live reconnect-triggered check (captured in logcat) reached
the same endpoint and got the same "no new version" answer our reconstructed
client gets. See research/opwwe231/2026-10-20-a192/ota-and-ohealth.json and
docs/ota-and-ohealth.md for that evidence.

Field notes from the jadx pass (e0.f, decompiled):
  - otaPrefix / productName: deviceModel directly, UNLESS DeviceModel.B6()
    is true, in which case it's OppoOtaUtils.e(deviceSku, deviceModel) (which
    only remaps two hardcoded SKUs, "OB19B3"/"OB19B1"; any other SKU,
    including the real deviceSku if different from the public model name
    "OPWWE231", passes through unchanged). B6() == a6() || o6(), each of
    which reads a Kotlin `by lazy` boolean field whose initializer was not
    resolved by this pass (JADX does not expose it in the constructor body).
    This script always sends the plain model name; whether the real app
    does too for this device is UNRESOLVED.
  - otaVersion: deviceOtaVersion directly, UNLESS the same B6() is true, in
    which case it's "{otaPrefix}_{deviceHardVersion}.{deviceSoftVersion}".
    Same unresolved flag as above.
  - romVersion: deviceSoftVersion, a value sourced from the Bluetooth DM
    protocol link to the watch, NOT necessarily the short "A.xxx" form this
    script has been using (no direct getprop equivalent is known).
  - imei: NOT a telephony IMEI. It is
    ig.w0.buildByMac(deviceBtMac).getDeviceUniqueId(), which looks up the
    paired device's locally-cached record and returns either
    UserDeviceInfo.getRm() (likely "region mark" -- ro.vendor.oplus.regionmark
    on this watch reads "EUEX") or .getDeviceSn() (a device serial), chosen
    by another unresolved capability flag. The default here is "EUEX" as the
    best-grounded guess for Rm; it is NOT confirmed to be the literal value
    the real app sends.

None of this changes the conclusion already documented: four independent
checks (the device's real current version via this reconstruction, two A.190
variants, and the real app's own live check) all returned "no new version"
with no format/validation error. The corrections above matter for anyone
extending this tool or re-deriving the protocol, not for that conclusion.

Usage:
  python3 oppo_ota_query.py --body-only --region eu \
      --ota-version <ro.build.version.ota> --rom-version <A.xxx> > body.json
  curl -s -X POST -H "Content-Type: application/json; charset=UTF-8" \
      --data @body.json https://ifota-term-eu.coloros.com/post/Query_Update
  python3 oppo_ota_query.py --decrypt-resps "<resps field from the response>"

Sweep mode (sends one real request per version in KNOWN_OLDER_VERSIONS,
declaring the device as older than it really is, to check whether the
server then offers a download). This does contact the vendor server for
each version, for real:
  python3 oppo_ota_query.py --sweep --send --region eu \
      --ota-version <ro.build.version.ota>

Four independent checks already done on 20 October 2026 (the device's real
current version, two A.190 variants, and the real paired app's own live
reconnect check) all returned "no new version" with no format/validation
error ever returned for any declared version. See docs/ota-and-ohealth.md
for why that pattern points to this endpoint having no catalog entry for
this product at all, independent of which version is declared -- sweep
mode is provided for completeness, not because it is expected to find a
different answer.

Only needs Python 3's standard library plus the system `openssl` binary.
"""
import argparse
import base64
import hashlib
import json
import re
import secrets
import string
import subprocess
import sys
import time
import urllib.error
import urllib.request

# romVersions seen in public forum/dump references as of 20 October 2026
# (A.94 Telegram @android_dumps post, A.122 forum support post, A.172 Wear
# OS 5 community thread, A.190 this device's own recovery log). None is
# confirmed to still be in this server's catalog; see docs/ota-and-ohealth.md
# for why the evidence so far points to this endpoint having no catalog for
# this product at all, independent of which version is declared.
KNOWN_OLDER_VERSIONS = ["A.190", "A.172", "A.144", "A.122", "A.94"]

KEY_TABLE = ["oppo1997", "baed2017", "java7865", "231uiedn", "09e32ji6",
             "0oiu3jdy", "0pej387l", "2dkliuyt", "20odiuye", "87j3id7w"]
HOSTS = {
    "eu": "https://ifota-term-eu.coloros.com/post/",
    "us": "https://ifota-term-us.allawnos.com/post/",
    "in": "https://ifota-term-in.coloros.com/post/",
    "default": "https://ifota-term.coloros.com/post/",
}


def derive(rnd: str):
    """rf.a.e: key = TABLE[rnd[0]] + rnd[4:12]; IV = bytes.fromhex(md5hex(key))."""
    key = (KEY_TABLE[int(rnd[0])] + rnd[4:12]).encode()
    return key, hashlib.md5(key).digest()


def ctr(data: bytes, key: bytes, iv: bytes) -> bytes:
    out = subprocess.run(
        ["openssl", "enc", "-aes-128-ctr", "-K", key.hex(), "-iv", iv.hex()],
        input=data, capture_output=True, check=True)
    return out.stdout


def new_random() -> str:
    alphabet = string.ascii_letters + string.digits
    return str(secrets.randbelow(10)) + "".join(secrets.choice(alphabet) for _ in range(14))


def encrypt_body(plain: dict, rnd: str = None) -> dict:
    rnd = rnd or new_random()
    key, iv = derive(rnd)
    ct = base64.b64encode(ctr(json.dumps(plain, separators=(",", ":")).encode(), key, iv)).decode()
    return {"params": ct + rnd, "version": "4"}


def decrypt_resps(resps: str) -> dict:
    ct, rnd = resps[:-15], resps[-15:]
    key, iv = derive(rnd)
    return json.loads(ctr(base64.b64decode(ct), key, iv))


def derive_ota_version(template: str, rom_version: str) -> str:
    """Swap the A.<n> token inside a real otaVersion string for a different
    romVersion; the build counter and timestamp stay the template's. Same
    logic as ota_check_menu.py's helper, duplicated so this file works
    standalone."""
    token = rom_version if re.fullmatch(r"A\.\d+", rom_version) else f"A.{rom_version.lstrip('A.')}"
    new, n = re.subn(r"A\.\d+", token, template, count=1)
    return new if n else template


def build_request(a, ota_version=None, rom_version=None) -> dict:
    ota_version = ota_version or a.ota_version
    rom_version = rom_version or a.rom_version
    return {
        "version": "3",
        "otaPrefix": a.prefix or a.product,
        "productName": a.product,
        "otaVersion": ota_version,
        "romVersion": rom_version,
        "imei": a.imei,
        "mode": 0,
        "language": a.language,
        "time": int(time.time() * 1000),
        "androidVersion": "unknown",
        "colorOSVersion": "unknown",
        "registrationId": "unknown",
        "type": "0",
        "modules": [{"otaPkgType": "1", "version": ota_version}],
    }


def send_one(a, region, plain) -> dict:
    """Encrypt, POST, decrypt. Returns a result dict; never raises on HTTP/parse errors."""
    body = encrypt_body(plain)
    url = HOSTS[region] + "Query_Update"
    req = urllib.request.Request(url, data=json.dumps(body).encode(),
                                  headers={"Content-Type": "application/json; charset=UTF-8"},
                                  method="POST")
    try:
        with urllib.request.urlopen(req, timeout=30) as r:
            raw, status = r.read().decode(), r.status
    except urllib.error.HTTPError as e:
        raw, status = e.read().decode(errors="replace"), e.code
    except urllib.error.URLError as e:
        return {"romVersion": plain["romVersion"], "http": None, "error": str(e)}
    try:
        outer = json.loads(raw)
    except ValueError:
        return {"romVersion": plain["romVersion"], "http": status, "error": "non-JSON response", "raw": raw[:300]}
    if outer.get("resps"):
        try:
            decrypted = decrypt_resps(outer["resps"])
        except Exception as e:
            return {"romVersion": plain["romVersion"], "http": status, "error": f"decrypt failed: {e}"}
        return {"romVersion": plain["romVersion"], "http": status, "decrypted": decrypted}
    return {"romVersion": plain["romVersion"], "http": status, "raw": outer}


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--region", choices=HOSTS, default="eu")
    p.add_argument("--product", default="OPWWE231")
    p.add_argument("--prefix")
    p.add_argument("--ota-version")
    p.add_argument("--rom-version")
    p.add_argument("--imei", default="EUEX",
                    help="NOT a telephony IMEI despite the field name; the real app sends "
                         "UserDeviceInfo.getRm() or .getDeviceSn() (see module docstring). "
                         "Default is this watch's regionmark (best-grounded guess for Rm, "
                         "unconfirmed). Server rejects an empty value outright.")
    p.add_argument("--language", default="en-US")
    p.add_argument("--selftest", action="store_true", help="local round-trip only, no network")
    p.add_argument("--send", action="store_true", help="actually POST to the vendor server")
    p.add_argument("--body-only", action="store_true",
                    help="print just the encrypted request JSON to stdout (for use with curl), no network")
    p.add_argument("--decrypt-resps", metavar="RESPS_STRING",
                    help="decrypt a captured 'resps' value instead of building a request")
    p.add_argument("--sweep", action="store_true",
                    help="send one real request per version in KNOWN_OLDER_VERSIONS "
                         "(requires --ota-version as the real template and --send); "
                         "prints a one-line summary per version, then a final table")
    a = p.parse_args()

    if a.decrypt_resps:
        print(json.dumps(decrypt_resps(a.decrypt_resps), indent=2))
        return

    if a.sweep:
        if not a.send:
            p.error("--sweep requires --send (this does contact the vendor server)")
        if not a.ota_version:
            p.error("--sweep requires --ota-version: the device's real current "
                     "otaVersion string, used as the template for each older version")
        results = []
        for v in KNOWN_OLDER_VERSIONS:
            ota = derive_ota_version(a.ota_version, v)
            plain = build_request(a, ota_version=ota, rom_version=v)
            print(f"-> romVersion={v}  otaVersion={ota}", file=sys.stderr)
            res = send_one(a, a.region, plain)
            results.append(res)
            if "decrypted" in res:
                print(f"   HTTP {res['http']}: {json.dumps(res['decrypted'])}", file=sys.stderr)
            else:
                print(f"   HTTP {res['http']}: {res.get('error') or res.get('raw')}", file=sys.stderr)
            time.sleep(1.5)  # don't hammer the server back-to-back
        print(json.dumps(results, indent=2))
        return

    if not a.ota_version or not a.rom_version:
        p.error("--ota-version and --rom-version are required unless using --decrypt-resps or --sweep")

    plain = build_request(a)
    body = encrypt_body(plain)
    # self-check: our own ciphertext must decrypt back to the plaintext
    rnd = body["params"][-15:]
    back = json.loads(ctr(base64.b64decode(body["params"][:-15]), *derive(rnd)))
    assert back == plain, "round-trip mismatch"

    if a.body_only:
        print(json.dumps(body))
        return

    print("request plaintext:", json.dumps(plain), file=sys.stderr)
    if a.selftest or not a.send:
        print("round-trip OK (not sent)", file=sys.stderr)
        return

    url = HOSTS[a.region] + "Query_Update"
    req = urllib.request.Request(url, data=json.dumps(body).encode(),
                                  headers={"Content-Type": "application/json; charset=UTF-8"},
                                  method="POST")
    try:
        with urllib.request.urlopen(req, timeout=30) as r:
            raw = r.read().decode()
            status = r.status
    except urllib.error.HTTPError as e:
        raw, status = e.read().decode(errors="replace"), e.code
    print("HTTP", status, file=sys.stderr)
    try:
        outer = json.loads(raw)
    except ValueError:
        print("non-JSON response:", raw[:500])
        return
    print("outer keys:", {k: (v if k != "resps" else f"<{len(v)} chars>") for k, v in outer.items()}, file=sys.stderr)
    if "resps" in outer and outer["resps"]:
        print(json.dumps(decrypt_resps(outer["resps"]), indent=2))
    else:
        print(json.dumps(outer, indent=2))


if __name__ == "__main__":
    main()
