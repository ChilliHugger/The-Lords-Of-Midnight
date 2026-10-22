#!/usr/bin/env bash
# Local release builds, replacing the manual steps in docs/deployment_process.md.
#   scripts/release.sh <lom|ddr|citadel> <ios|mac|android|wasm|all> [--upload]
# Applies VERSION first (scripts/set_version.sh), then for each platform:
#   ios, mac  regenerate the Xcode project (same folder, not wiped), archive in RelWithDebInfo and
#             archive as "<game title>.xcarchive" and export a signed .ipa/.pkg, both in
#             Builds/release/<app>/<version>/<platform>
#             (--upload sends it to App Store Connect instead, symbols included)
#   android   builds the signed .aab (keystore settings come from ~/.gradle/signing-<game>.properties)
#   wasm      builds only; deploying the web build is the "Web build & deploy" workflow
# dSYMs are copied to dSYMs-<platform>/ next to each export. Run from anywhere.
# Still manual: release notes, TestFlight testers and promoting builds in the store consoles.
set -euo pipefail
cd "$(dirname "$0")/.."

game=${1:?usage: release.sh <lom|ddr|citadel> <ios|mac|android|wasm|all> [--upload]}
platform=${2:?usage: release.sh <lom|ddr|citadel> <ios|mac|android|wasm|all> [--upload]}
upload=0; [ "${3:-}" = "--upload" ] && upload=1

case $game in
    lom)     tme=LOM;     app=midnight; title="The Lords of Midnight"; android=proj.android_midnight ;;
    ddr)     tme=DDR;     app=revenge;  title="Doomdark's Revenge";   android=proj.android_revenge ;;
    citadel) tme=CITADEL; app=citadel;  title="The Citadel";         android=proj.android_citadel ;;
    *) echo "unknown game '$game'" >&2; exit 2 ;;
esac
TEAM_ID=8K6PYTU8T7

scripts/set_version.sh
read -r version build < VERSION
out="$PWD/Builds/release/$app/$version.$build"
mkdir -p "$out"

# Re-runs the cmake configure in the existing folder (no wipe): the resource and source lists are
# globbed at configure time, so a project generated before files were added or removed is stale.
apple() {
    local plat=$1 dir=Builds/$1-$game args method
    if [ "$plat" = ios ]; then args="-p ios -a arm64"; method=app-store-connect
    else args="-p osx -a x64"; method=app-store-connect; fi
    axmol build -configOnly $args -xc "-DTME=$tme,-B$dir"

    mkdir -p "$out/$plat"
    local archive="$out/$plat/$title.xcarchive" opts="$out/ExportOptions-$plat.plist"
    xcodebuild -project "$dir/$app.xcodeproj" -scheme "$app" -configuration RelWithDebInfo \
        -destination "generic/platform=$([ $plat = ios ] && echo iOS || echo macOS)" \
        -archivePath "$archive" -allowProvisioningUpdates \
        DEBUG_INFORMATION_FORMAT=dwarf-with-dsym archive
    # the name Xcode Organizer shows; the generated scheme has no Archive Name, so it says "$app"
    /usr/libexec/PlistBuddy -c "Set :Name $title" "$archive/Info.plist"

    cat > "$opts" <<PLIST
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0"><dict>
  <key>method</key><string>$method</string>
  <key>teamID</key><string>$TEAM_ID</string>
  <key>signingStyle</key><string>automatic</string>
  <key>uploadSymbols</key><true/>
  <key>destination</key><string>$([ $upload = 1 ] && echo upload || echo export)</string>
</dict></plist>
PLIST
    xcodebuild -exportArchive -archivePath "$archive" -exportOptionsPlist "$opts" \
        -exportPath "$out/$plat" -allowProvisioningUpdates
    mkdir -p "$out/dSYMs-$plat" && cp -R "$archive/dSYMs/"*.dSYM "$out/dSYMs-$plat/"
}

# Each game has its own keystore. Its settings live outside the repo in
# ~/.gradle/signing-<game>.properties (RELEASE_STORE_FILE, RELEASE_STORE_PASSWORD,
# RELEASE_KEY_ALIAS, RELEASE_KEY_PASSWORD). They reach Gradle as environment variables rather
# than -P arguments so the passwords don't show up in the process list. Gradle builds an
# unsigned bundle without complaint when they are missing, hence the explicit checks.
load_android_signing() {
    local f="${GRADLE_USER_HOME:-$HOME/.gradle}/signing-$game.properties" line key
    [ -f "$f" ] || { echo "Missing $f (RELEASE_STORE_FILE/_PASSWORD/KEY_ALIAS/KEY_PASSWORD)" >&2; exit 1; }
    while IFS= read -r line || [ -n "$line" ]; do
        [[ -z $line || $line == \#* ]] && continue
        export "ORG_GRADLE_PROJECT_$line"
    done < "$f"
    for key in STORE_FILE STORE_PASSWORD KEY_ALIAS KEY_PASSWORD; do
        [ -n "$(eval "echo \${ORG_GRADLE_PROJECT_RELEASE_$key:-}")" ] || { echo "$f is missing RELEASE_$key" >&2; exit 1; }
    done
    [ -f "$ORG_GRADLE_PROJECT_RELEASE_STORE_FILE" ] || { echo "Keystore not found: $ORG_GRADLE_PROJECT_RELEASE_STORE_FILE" >&2; exit 1; }
}

droid() {
    load_android_signing
    # Gradle needs JDK 17+; the system default here is older, so use Android Studio's bundled one.
    local jbr="/Applications/Android Studio.app/Contents/jbr/Contents/Home"
    [ -d "$jbr" ] && export JAVA_HOME="$jbr"
    # local.properties (sdk.dir) is per-machine and ignored by git, so don't depend on it existing
    [ -z "${ANDROID_HOME:-}" ] && [ -d "$HOME/Library/Android/sdk" ] && export ANDROID_HOME="$HOME/Library/Android/sdk"
    (cd "$android" && ./gradlew clean bundleRelease)
    cp "$android"/app/build/outputs/bundle/release/*.aab "$out/"
}

wasm() { scripts/wasm_builds.sh "$tme" "$version" "$build"; }

for p in $([ "$platform" = all ] && echo "ios mac android" || echo "$platform"); do
    case $p in
        ios|mac) apple "$p" ;;
        android) droid ;;
        wasm)    wasm ;;
        *) echo "unknown platform '$p'" >&2; exit 2 ;;
    esac
done
echo "Release output: $out"
