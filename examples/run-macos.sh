#!/usr/bin/env bash
set -euo pipefail
export BRIDGE_USERNAME="<your-username>"
export BRIDGE_PASSWORD="<your-password>"
./bin/macos-arm64/serial_bridge --stdio --rest-url http://127.0.0.1:5000 --username-env BRIDGE_USERNAME --password-env BRIDGE_PASSWORD
