#!/bin/bash

# Build script for NodeMCU project
# This script increments the build number before compilation

echo "🔨 NodeMCU Build Script"
echo "======================="

# Increment build number
echo "📈 Incrementing build number..."
python3 increment_build.py

if [ $? -eq 0 ]; then
    echo "✅ Build number incremented successfully"
    echo ""
    echo "📋 Build Information:"
    echo "--------------------"
    # Extract and display current build info
    grep "BUILD_NUMBER" main/build_info.h
    grep "FIRMWARE_VERSION" main/build_info.h
    echo ""
    echo "🚀 Ready for compilation!"
    echo ""
    echo "Next steps:"
    echo "1. Open Arduino IDE"
    echo "2. Open main/main.ino"
    echo "3. Compile and upload to your NodeMCU"
    echo ""
    echo "Or use Arduino CLI:"
    echo "arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 main/"
else
    echo "❌ Failed to increment build number"
    exit 1
fi
