# Publishing device evidence

We publish reviewed technical findings in `research/` and keep original
acquisitions under the ignored `backups/` directory. Raw acquisitions can contain
identifiers, account state, notifications, health information and credentials.

## Included

- An explicit selection of model, firmware, ABI, security and access properties.
- Partition names, block paths, types and reported lengths.
- Selected literal ADB, fastboot and recovery output, with original line numbers.
- Technical conclusions from focused application analysis, with aggregate log counts.
- Firmware-family fingerprints, package hashes, public source links and known limitations.

Build fingerprints identify a software build shared by devices. We retain them
for firmware matching; they are distinct from a hardware serial or Android ID.
Partition names such as `persist` and `keystore` describe layout only. We publish
no contents from those partitions.

## Removed or kept private

We omit watch and phone serials, Android IDs, Google check-in identifiers and
secrets, account data, MAC/IP addresses, Wi-Fi names, local user paths, device
names chosen by the owner, tokens and private download URLs. We also exclude
health/activity records, notifications, UI captures, unfiltered bugreports and
phone logs, decrypted payloads, decoder keys, APKs and firmware binaries.

In the fastboot excerpt, `[REDACTED_DEVICE_SERIAL]` replaces the serial value.
Other excerpts select exact non-sensitive lines; JSON exports select individual
fields and aggregate counts. These are curated exports, not whole log files
passed through an automatic search-and-replace filter.

The public source manifest names private source files by relative path and
records their SHA-256 digests and export transformations. Source files remain
private. The public `SHA256SUMS` covers exported evidence; it does not certify
firmware authenticity, completeness of acquisition or restoration capability.

## Reviewing future additions

Select the smallest evidence needed to support a finding. Inspect both the
selected values and their context before adding them to `research/`. Check for
identifiers and secrets, validate links and file formats, and regenerate the
collection's checksums after a reviewed change. Keep capture date, model/build,
source location, transformation and uncertainty with the evidence.

We preserve original source contents, including historical labels. Renaming the
private parent directory does not rewrite captured device output or manifests.
The repository and current research target use **OnePlus Watch 2 / OPWWE231**.
