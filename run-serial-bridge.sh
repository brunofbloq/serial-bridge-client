#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

BRIDGE_BIN="${BRIDGE_BIN:-./bin/linux-x64/serial_bridge}"
SERVER_URL="${BRIDGE_SERVER_URL:-http://127.0.0.1:5000}"
PTY_LINK="${BRIDGE_PTY_LINK:-/tmp/serial_bridge}"
AUTH_FILE="${BRIDGE_AUTH_FILE:-}"

# If an auth file is provided, let it be the single auth source.
if [[ -z "$AUTH_FILE" ]]; then
  export BRIDGE_USERNAME="${BRIDGE_USERNAME:-}"
  export BRIDGE_PASSWORD="${BRIDGE_PASSWORD:-}"
fi

if ! command -v socat >/dev/null 2>&1; then
  echo "Error: socat is not installed or not in PATH." >&2
  exit 1
fi

if [[ ! -x "$BRIDGE_BIN" ]]; then
  echo "Error: bridge binary not executable: $BRIDGE_BIN" >&2
  echo "Try: chmod +x $BRIDGE_BIN" >&2
  exit 1
fi

if [[ -n "$AUTH_FILE" && ! -f "$AUTH_FILE" ]]; then
  echo "Error: auth file not found: $AUTH_FILE" >&2
  exit 1
fi

if [[ -z "$AUTH_FILE" ]]; then
  if [[ -z "${BRIDGE_TOKEN:-}" && ( -z "${BRIDGE_USERNAME:-}" || -z "${BRIDGE_PASSWORD:-}" ) ]]; then
    echo "Error: set BRIDGE_TOKEN or BRIDGE_USERNAME/BRIDGE_PASSWORD, or provide BRIDGE_AUTH_FILE" >&2
    exit 1
  fi
fi

BRIDGE_BIN_ABS="$(cd "$(dirname "$BRIDGE_BIN")" && pwd)/$(basename "$BRIDGE_BIN")"
TMP_BRIDGE_BIN="/tmp/serial_bridge_$$"
ln -sf "$BRIDGE_BIN_ABS" "$TMP_BRIDGE_BIN"
printf -v SERVER_URL_Q '%q' "$SERVER_URL"
SYSTEM_CMD="$TMP_BRIDGE_BIN --stdio --rest-url $SERVER_URL_Q --token-env BRIDGE_TOKEN --username-env BRIDGE_USERNAME --password-env BRIDGE_PASSWORD"

if [[ -n "$AUTH_FILE" ]]; then
  printf -v AUTH_FILE_Q '%q' "$AUTH_FILE"
  SYSTEM_CMD="$SYSTEM_CMD --auth-file $AUTH_FILE_Q"
fi

echo "Starting virtual serial bridge..."
echo "  PTY link:  $PTY_LINK"
echo "  REST URL:  $SERVER_URL"
echo "  Binary:    $BRIDGE_BIN"
if [[ -n "$AUTH_FILE" ]]; then
  echo "  Auth:      file ($AUTH_FILE)"
else
  echo "  Auth:      env (BRIDGE_USERNAME/PASSWORD or BRIDGE_TOKEN)"
fi

exec socat "pty,raw,echo=0,echoctl=0,link=$PTY_LINK" \
  "SYSTEM:'$SYSTEM_CMD'"
