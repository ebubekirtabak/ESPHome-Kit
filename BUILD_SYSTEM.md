# NodeMCU Build System

This project includes an automated build numbering system that tracks firmware builds.

## Build Information

The system tracks:
- **Build Number**: Auto-incremented integer for each build
- **Firmware Version**: Semantic version (e.g., "1.0.0")
- **Build Date/Time**: When the firmware was compiled
- **Additional Info**: Free heap, uptime, chip ID, etc.

## Files

- `main/build_info.h` - Contains build constants
- `increment_build.py` - Python script to increment build number
- `build.sh` - Complete build script

## Usage

### Option 1: Manual Build Number Increment
```bash
# Before each build, run:
python3 increment_build.py

# Then compile in Arduino IDE
```

### Option 2: Automated Build Script
```bash
# Run the complete build script:
./build.sh

# Then compile in Arduino IDE
```

### Option 3: Arduino CLI (if available)
```bash
# Complete automated build and upload:
./build.sh
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 main/
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp8266:esp8266:nodemcuv2 main/
```

## API Response

The `/api/status` endpoint now returns:
```json
{
  "connected": true,
  "ssid": "MyWiFi",
  "ip": "192.168.1.184",
  "rssi": -45,
  "firmwareVersion": "1.0.0",
  "buildNumber": 5,
  "buildDate": "Jul  5 2025",
  "buildTime": "12:23:55",
  "freeHeap": 45216,
  "uptime": 123456,
  "chipId": "ABC123"
}
```

## Dashboard Display

The build information is displayed in the Dashboard under "System Information":
- Firmware version and build number
- Build date and time
- System hardware info
- Current status and uptime

## Customization

Edit `main/build_info.h` to change:
- `FIRMWARE_VERSION` - Semantic version string
- Initial `BUILD_NUMBER` (usually leave at 1)

The build date/time uses compiler macros `__DATE__` and `__TIME__` automatically.
