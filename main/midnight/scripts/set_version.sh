#!/usr/bin/env bash
# Single place to set the version for every platform and game.
#   scripts/set_version.sh              apply ./VERSION to all files
#   scripts/set_version.sh 2.0.12 57    write ./VERSION, then apply it
#   scripts/set_version.sh --check      exit 1 if any file disagrees with ./VERSION
# VERSION holds "<major>.<minor>.<patch> <build>". Run from main/midnight (or anywhere).
# Uses perl rather than sed -i so it behaves the same on macOS and Git Bash.
set -euo pipefail
cd "$(dirname "$0")/.."

check=0
if [ "${1:-}" = "--check" ]; then check=1; shift; fi
[ $# -eq 2 ] && printf '%s %s\n' "$1" "$2" > VERSION

read -r ver build < VERSION
[[ $ver =~ ^([0-9]+)\.([0-9]+)\.([0-9]+)$ ]] || { echo "bad version '$ver'" >&2; exit 2; }
maj=${BASH_REMATCH[1]} min=${BASH_REMATCH[2]} pat=${BASH_REMATCH[3]}
[[ $build =~ ^[0-9]+$ ]] || { echo "bad build '$build'" >&2; exit 2; }
code=$((maj * 1000000 + min * 10000 + build))   # android versionCode, e.g. 2.0.x build 56 -> 2000056
rc="$maj,$min,$pat,$build"; rcs="$maj, $min, $pat, $build"

fail=0
# apply FILE PERL_SUBSTITUTION: rewrites in place (or, with --check, just compares)
apply() {
    local file=$1 expr=$2 tmp
    tmp=$(mktemp)
    perl -0pe "$expr" "$file" > "$tmp"
    if ! cmp -s "$file" "$tmp"; then
        if [ $check -eq 1 ]; then echo "out of date: $file"; fail=1; else cat "$tmp" > "$file"; echo "updated: $file"; fi
    fi
    rm -f "$tmp"
}

for app in midnight revenge citadel; do
    apply proj.win32/$app/game.rc \
        "s/(FILEVERSION |PRODUCTVERSION )\d+,\d+,\d+,\d+/\${1}$rc/g; s/(\"(?:File|Product)Version\", \")[^\\\\\"]*/\${1}$rcs/g"
    for plist in proj.ios_mac/mac/$app/Info.plist proj.ios_mac/ios/targets/ios/$app/Info.plist; do
        apply "$plist" \
            "s|(<key>CFBundleShortVersionString</key>\s*<string>)[^<]*|\${1}$ver|; s|(<key>CFBundleVersion</key>\s*<string>)[^<]*|\${1}$build|"
    done
    apply proj.android_$app/app/build.gradle \
        "s/(versionCode )\d+/\${1}$code/; s/(versionName \")[^\"]*/\${1}$ver/"
done

apply proj.wasm/version.cmake \
    "s/(set\(WASM_APP_VERSION \")[^\"]*/\${1}$ver/; s/(set\(WASM_APP_BUILD_NO \")[^\"]*/\${1}$build/"

[ $check -eq 1 ] && [ $fail -eq 0 ] && echo "all files match $ver ($build)"
exit $fail
