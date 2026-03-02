# Linux Runtime Checklist

Use this checklist on a fresh Linux machine to verify `serial_bridge` can run.

## 1. Platform compatibility

- CPU/OS must be Linux `x86_64`.
- Confirm binary type:

```bash
file ./bin/linux-x64/serial_bridge
```

## 2. Required shared libraries

Check dynamic dependencies:

```bash
ldd ./bin/linux-x64/serial_bridge
```

All entries must resolve. No `not found` lines.

## 3. Install runtime tools

Install `socat`:

```bash
sudo apt update && sudo apt install -y socat
```

## 4. Make executables runnable

```bash
chmod +x ./bin/linux-x64/serial_bridge
chmod +x ./run-serial-bridge.sh
```

## 5. Configure URL and auth

Choose one auth method.
If using username/password, retrieve them from:
- `${BRIDGE_SERVER_URL}/user/login`

Option A: Username/password env vars

```bash
export BRIDGE_SERVER_URL="http://<api-host>:5000"
export BRIDGE_USERNAME="<username>"
export BRIDGE_PASSWORD="<password>"
```

Option B: Token env var

```bash
export BRIDGE_SERVER_URL="http://<api-host>:5000"
export BRIDGE_TOKEN="<bearer-token>"
```

Option C: JSON auth file

```bash
export BRIDGE_SERVER_URL="http://<api-host>:5000"
export BRIDGE_AUTH_FILE="./bridge-auth.json"
```

`bridge-auth.json` examples:

```json
{"username":"<your-username>","password":"<your-password>"}
```

```json
{"token":"<bearer-token>"}
```

## 6. Start bridge

```bash
./run-serial-bridge.sh
```

## 7. Validate runtime

In a second terminal:

```bash
ls -la /tmp/serial_bridge
curl -i "$BRIDGE_SERVER_URL"
```

- `/tmp/serial_bridge` must exist.
- API URL must be reachable.
