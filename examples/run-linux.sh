#!/usr/bin/env bash
set -euo pipefail
export KERONG_BRIDGE_USERNAME=bridge
export KERONG_BRIDGE_PASSWORD=bridge123
./bin/linux-x64/serial_bridge --stdio --rest-url http://127.0.0.1:5000
