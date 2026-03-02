#!/usr/bin/env python3
"""
Login example for Kerong emulator API.

Flow:
1) POST /login with username/password.
2) Extract access_token (JWT).
3) Call a protected endpoint with Authorization: Bearer <token>.
"""

import json
import os
import sys
from urllib import error, request


def _require_env(name: str) -> str:
    value = os.environ.get(name, "").strip()
    if not value:
        raise RuntimeError(f"missing required env var: {name}")
    return value


def _post_json(url: str, payload: dict, headers: dict | None = None) -> tuple[int, dict]:
    raw = json.dumps(payload).encode("utf-8")
    req_headers = {"Content-Type": "application/json"}
    if headers:
        req_headers.update(headers)
    req = request.Request(url, data=raw, headers=req_headers, method="POST")
    try:
        with request.urlopen(req, timeout=10) as resp:
            return resp.status, json.loads(resp.read().decode("utf-8"))
    except error.HTTPError as exc:
        body = exc.read().decode("utf-8", errors="replace")
        raise RuntimeError(f"POST {url} failed: status={exc.code} body={body}") from exc


def _get_json(url: str, headers: dict | None = None) -> tuple[int, dict]:
    req = request.Request(url, headers=headers or {}, method="GET")
    try:
        with request.urlopen(req, timeout=10) as resp:
            return resp.status, json.loads(resp.read().decode("utf-8"))
    except error.HTTPError as exc:
        body = exc.read().decode("utf-8", errors="replace")
        raise RuntimeError(f"GET {url} failed: status={exc.code} body={body}") from exc


def main() -> int:
    base = _require_env("BRIDGE_SERVER_URL").rstrip("/")
    username = _require_env("BRIDGE_USERNAME")
    password = _require_env("BRIDGE_PASSWORD")

    status, login_data = _post_json(
        f"{base}/login",
        {"username": username, "password": password},
    )
    token = login_data.get("access_token", "")
    if status != 200 or not token:
        raise RuntimeError(f"/login did not return access_token: {login_data}")

    print("login status:", status)
    print("jwt prefix:", token[:24] + "...")

    auth_headers = {"Authorization": f"Bearer {token}"}
    protected_status, bus_data = _get_json(f"{base}/bus", headers=auth_headers)
    print("protected status:", protected_status)
    print("protected payload:", json.dumps(bus_data))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # pragma: no cover
        print(f"ERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)
