#!/bin/bash
# Regenerate an existing Xcode project in place (picks up added/removed sources and shaders)
# without wiping the build dir, so builds stay incremental.
#
# usage: scripts/refresh_xcode.sh mac-lom   (also accepts mac_lom)

if [ -z "$1" ]; then
    echo "usage: $0 <build-name>   e.g. mac-lom, ios-citadel, mac-ddr"
    exit 1
fi

root="$(cd "$(dirname "$0")/.." && pwd)"
build="$root/Builds/${1//_/-}"

if [ ! -f "$build/CMakeCache.txt" ]; then
    echo "No configured build at $build - use the osx_builds_*.sh scripts to create it"
    exit 1
fi

cd "$build" && cmake .
