# Auth Login Examples (Python + C++)

These examples show how to:
1. Call `POST /login` with `username/password`.
2. Extract and keep the returned JWT `access_token`.
3. Call a protected endpoint (`GET /bus`) with `Authorization: Bearer <token>`.

## 0) Get credentials

Retrieve your user credentials from:

`$BRIDGE_SERVER_URL/user/login`

Example:

`https://kerong-modbus-emulator.vercel.app/user/login`

## 1) Set environment variables

```bash
export BRIDGE_SERVER_URL="https://kerong-modbus-emulator.vercel.app"
export BRIDGE_USERNAME="<your-username>"
export BRIDGE_PASSWORD="<your-password>"
```

## 2) Python example

```bash
python3 examples/auth_login/python_login.py
```

## 3) C++ example

Build:

```bash
g++ -std=c++17 -O2 examples/auth_login/cpp_login.cpp -lcurl -o /tmp/cpp_login
```

Run:

```bash
/tmp/cpp_login
```
