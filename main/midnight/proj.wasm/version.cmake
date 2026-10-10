# Web build version - kept alongside the other platforms' versions
# (Info.plist CFBundleShortVersionString/CFBundleVersion, game.rc, build.gradle).
# Override per build with -DWASM_APP_VERSION=2.0.12 -DWASM_APP_BUILD_NO=57
# (e.g. axmol build -p wasm -xc '-DTME=LOM,-DWASM_APP_VERSION=2.0.12,-DWASM_APP_BUILD_NO=57').
if(NOT DEFINED WASM_APP_VERSION)
    set(WASM_APP_VERSION "2.0.11")
endif()
if(NOT DEFINED WASM_APP_BUILD_NO)
    set(WASM_APP_BUILD_NO "56")
endif()
