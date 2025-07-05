#!/usr/bin/env python3
"""
Build number incrementer for NodeMCU project
Run this script before each build to increment the build number
"""

import os
import re
from datetime import datetime

def increment_build_number():
    build_info_path = os.path.join(os.path.dirname(__file__), 'main', 'build_info.h')

    if not os.path.exists(build_info_path):
        print(f"Error: {build_info_path} not found!")
        return

    with open(build_info_path, 'r') as f:
        content = f.read()

    build_match = re.search(r'#define BUILD_NUMBER (\d+)', content)
    if build_match:
        current_build = int(build_match.group(1))
        new_build = current_build + 1
    else:
        new_build = 1

    new_content = re.sub(
        r'#define BUILD_NUMBER \d+',
        f'#define BUILD_NUMBER {new_build}',
        content
    )

    now = datetime.now()
    build_timestamp = now.strftime("%Y-%m-%d %H:%M:%S")

    with open(build_info_path, 'w') as f:
        f.write(new_content)

    print(f"Build number incremented: {current_build if build_match else 0} -> {new_build}")
    print(f"Build timestamp: {build_timestamp}")
    print(f"Updated: {build_info_path}")

if __name__ == "__main__":
    increment_build_number()
