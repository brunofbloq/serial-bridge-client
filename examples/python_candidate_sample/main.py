import serial

# Replace with the virtual serial path provided by your environment.
PORT = "/tmp/serial_bridge"

with serial.Serial(PORT, 19200, timeout=1) as ser:
    req = bytes.fromhex("01 03 00 0F 00 01 D5 CA")
    ser.write(req)
    resp = ser.read(64)
    print(resp.hex(" "))
