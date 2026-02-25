# serial-bridge-client

Distribution repository for developers/candidates consuming the `serial_bridge` executable.

## Scope

- Prebuilt binaries per OS.
- Quickstart docs.
- Auth/config examples.
- Candidate sample apps.

## WSL note

Use the Linux binary under `bin/linux-x64/` inside WSL.

## Release Handoff From `serial-bridge`

This repo receives compiled artifacts produced in `../serial-bridge`.

### 1) Build in `serial-bridge`

```bash
cd "/mnt/c/Users/Bruno Figueiredo/Documents/Bloqit/Challanges/serial-bridge"
python3 -m venv .venv
source .venv/bin/activate
bash packaging/scripts/build-linux.sh
```

Build Windows/macOS on native OS or via GitHub Actions release workflow.

### 2) Package and checksum (example: Linux)

```bash
VERSION=v0.1.0
mkdir -p release
tar -czf "release/serial-bridge-${VERSION}-linux-x64.tar.gz" -C dist serial_bridge_client.dist
(cd release && sha256sum "serial-bridge-${VERSION}-linux-x64.tar.gz" > "serial-bridge-${VERSION}-linux-x64.sha256")
```

### 3) Copy artifacts into `serial-bridge-client`

```bash
cd "/mnt/c/Users/Bruno Figueiredo/Documents/Bloqit/Challanges"
cp "serial-bridge/release/serial-bridge-${VERSION}-linux-x64.tar.gz" "serial-bridge-client/bin/"
cp "serial-bridge/release/serial-bridge-${VERSION}-linux-x64.sha256" "serial-bridge-client/"
```

Repeat for:

- `serial-bridge-${VERSION}-windows-x64.zip`
- `serial-bridge-${VERSION}-macos-arm64.tar.gz`
- Optional `serial-bridge-${VERSION}-macos-x64.tar.gz`

### 4) Update combined checksums

```bash
cd "/mnt/c/Users/Bruno Figueiredo/Documents/Bloqit/Challanges/serial-bridge-client"
cat *.sha256 > checksums.txt
```

### 5) Commit and publish `serial-bridge-client`

```bash
git add .
git commit -m "release: add serial-bridge ${VERSION} binaries and checksums"
git push
```
