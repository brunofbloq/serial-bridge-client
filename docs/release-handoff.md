# Release Handoff From `serial-bridge`

This document describes how compiled artifacts produced in the `serial-bridge` repo are packaged and published to this distribution repo (`serial-bridge-client`).

> **Automated path:** use the [Sync Artifacts](.github/workflows/sync-artifacts.yml) GitHub Action to do steps 3–5 automatically.

---

## 1) Build in `serial-bridge`

Clone/enter the `serial-bridge` repository and run the build for your target platform:

```bash
python3 -m venv .venv
source .venv/bin/activate
bash packaging/scripts/build-linux.sh
```

Build Windows/macOS on a native OS or via the `serial-bridge` GitHub Actions release workflow.

---

## 2) Package and checksum (example: Linux)

```bash
VERSION=v0.1.0
mkdir -p release
tar -czf "release/serial-bridge-${VERSION}-linux-x64.tar.gz" -C dist serial_bridge_client.dist
(cd release && sha256sum "serial-bridge-${VERSION}-linux-x64.tar.gz" > "serial-bridge-${VERSION}-linux-x64.sha256")
```

Repeat for:

- `serial-bridge-${VERSION}-windows-x64.zip`
- `serial-bridge-${VERSION}-macos-arm64.tar.gz`
- `serial-bridge-${VERSION}-macos-x64.tar.gz` *(optional)*

---

## 3) Copy artifacts into `serial-bridge-client`

```bash
cp serial-bridge/release/serial-bridge-${VERSION}-linux-x64.tar.gz   serial-bridge-client/bin/
cp serial-bridge/release/serial-bridge-${VERSION}-linux-x64.sha256   serial-bridge-client/bin/
```

Repeat for each platform archive and its corresponding `.sha256` file.

---

## 4) Update checksums

Generate a single combined checksum file from the raw binaries:

```bash
sha256sum bin/linux-x64/serial_bridge > bin/checksums.txt
```

Append additional platforms as they are copied in:

```bash
sha256sum bin/windows-x64/serial_bridge.exe >> bin/checksums.txt
sha256sum bin/macos-arm64/serial_bridge     >> bin/checksums.txt
sha256sum bin/macos-x64/serial_bridge       >> bin/checksums.txt
```

---

## 5) Commit and publish

```bash
git add bin/
git commit -m "release: add serial-bridge ${VERSION} binaries and checksums"
git push
```
