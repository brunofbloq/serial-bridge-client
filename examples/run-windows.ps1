$env:BRIDGE_USERNAME="<your-username>"
$env:BRIDGE_PASSWORD="<your-password>"
.\bin\windows-x64\serial_bridge.exe --stdio --rest-url http://127.0.0.1:5000 --username-env BRIDGE_USERNAME --password-env BRIDGE_PASSWORD
