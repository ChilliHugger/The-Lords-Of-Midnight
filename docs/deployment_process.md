### Deployment Process

## iOS
1. Make sure there is no LOM/DDR folder in resources (after Android build)
2. Update Bundle Version in info.plist
3. Generate Project
  - axmol build -configOnly -p ios -a arm64 -xc '-DTME=LOM,-BBuilds/ios-lom'
  - axmol build -configOnly -p ios -a arm64 -xc '-DTME=DDR,-BBuilds/ios-ddr'
4. Update Archive Name
  - Product | Scheme | Edit Scheme | Archive | Archive Name - "The Lords of Midnight" | "Doomdark's Revenge"
5. Check 'Archive Build Configuration'
  - Project | Scheme | Edit Scheme | Archive | Build Configuration - RelWithDebInfo  
6. Check Debug Info
  - Project 'ALL' | Build Settings | Build Options | Debug Information Format | RelWithDebInfo | Dwarf with dSym File
7. Check Build Version
  - Project 'Midnight' | General | Build
8. Build / Archive
9. Copy and compress
  - midnight.app.dsym from main/midnight/builds/ios-lom/bin/midnight/RelWithDebInfo to main/midnight/dsym files/ios/2.0.x.(xx)
  - revenge.app.dsym from main/midnight/builds/ios-ddr/bin/revenge/RelWithDebInfo to main/revenge/dsym files/ios/2.0.x.(xx)
10. Add dsym file to archive -
  - 'main/midnight/builds/iOS-lom/bin/midnight/RelWithDebInfo/midnight.app.dSYM'
  - 'main/midnight/builds/iOS-ddr/bin/revenge/RelWithDebInfo/revenge.app.dSYM'
11. Validate
12. Distribute
  - App Store Connect
  - Upload
  - Upload Symbols / Manage Version and Build Number
  - Automatically Manage Signing

## Add testers
1. When ready to submit - select and and Test Details
2. Add individual testers
3. TestFlight | iOS | External Testers | Builds
4. Add new build

## OS X
1. Make sure there is no LOM/DDR folder in resources (after Android build)
2. Update Bundle Version in info.plist
3. Generate Project
  - axmol build -configOnly -p osx -a x64 -xc '-DTME=LOM,-BBuilds/mac-lom'
  - axmol build -configOnly -p osx -a x64 -xc '-DTME=DDR,-BBuilds/mac-ddr'
4. Update Archive Name
  - Product | Scheme | Edit Scheme | Archive | Archive Name - "The Lords of Midnight" | "Doomdark's Revenge"
5. Check 'Archive Build Configuration'
  - Project | Scheme | Edit Scheme | Archive | Build Configuration - RelWithDebInfo  
6. Check Build Version
  - Project 'Midnight' | General | Build
7. Check Deployment Target - 10.11
8. Check Executable File '${EXECUTABLE_NAME}'
  - Project 'Midnight' | Info | Executable file
9. Check SDK - 'MacOS'
  - Project 'Midnight' | Build Settings | Architectures | MacOS
10. Check 'Hardened Runtime'
  - Project 'Midnight' | Build Settings | Signing | Enable Hardened Runtime
11. Check Debug Info
  - Project 'ALL' | Build Settings | Build Options | Debug Information Format | RelWithDebInfo | Dwarf with dSym File
12. Build / Archive
13. Add dsym file to archive
  - 'main/midnight/builds/mac-lom/bin/midnight/RelWithDebInfo/midnight.app.dSYM'
  - 'main/midnight/builds/mac-ddr/bin/revenge/RelWithDebInfo/revenge.app.dSYM'
14. Validate
15. Distribute App
  - App Store Connect
  - Upload
  - Upload Symbols
  - Automatically Manage Signing

## OS X Test Build
1. Distribute App
  - Developer Id
  - Upload
  - Upload Symbols
  - Automatically Manage Signing
  - Upload
2. Wait until "Ready to distribute"
3. Export Notarized App as 'The Lords of Midnight'

## Android - Google Play store
1. Make sure there is a or LOM/DDR folder in resources
2. Open proj.android in Android Studio and select the correct variant
3. Update versionName / versionCode in build.gradle
4. Check correct applicationId in build.gradle
5. Clean and ReBuild project
    - Build | Clean
    - Build | Rebuild
    - Build | Build Bundle(s) / APK(s) | Build Bundle(s)
8. Sign Bundle
    - Build | Generate Signed Bundle / APK
    - Android App Bundle
    - Next
    - Check Keystore path / Key Alias / Key password
    - Android Signing Keys/chillihugger.keystore
    - Next
    - Release
    - Finish
9. Locate Bundle - press locate on popup when signing finishes
    - midnight-release.aab
    - revenge-release.aab
    - check size (should be about 24mb otherwise you've forgotten the resources!)
10. Google Play Store - https://play.google.com/console/u/0/developers/5865908633896374109/
    - Internal Testing
    - Create New Release
    - Drag/Upload App Bundle
    - Version name - Version 2.0 (20000xx)
    - Release Notes ()
<en-GB>
Version 2.0 (xx)
</en-GB>
    - Next
    - Review Release
    - Start roll-out to Internal Testing
    - Promote to Alpha
    - Start roll-out to Alpha


## Scripts
Scripts can be run from the main folder
./scripts/osx_builds_lom.sh
./scripts/osx_builds_ddr.sh

These will create an ios, osx, and OSX test builds

## Debug DSYM files
atos -o midnight.app.dSYM -l 0x1000b4000 0x0000000100100008

## Versioning
main/midnight/proj.android_lom/app/build.gradle
main/midnight/proj.ios_mac/ios/midnight/Info.plist
main/midnight/proj.ios_mac/mac/midnight/Info.plist
main/midnight/proj.win32/midnight/game.rc
main/midnight/lom_v2.0_osx.cfg
main/midnight/lom_v2.0_win.cfg

main/midnight/proj.android_ddr/app/build.gradle
main/midnight/proj.ios_mac/ios/revenge/Info.plist
main/midnight/proj.ios_mac/mac/revenge/Info.plist
main/midnight/proj.win32/revenge/game.rc
main/midnight/ddr_v2.0_osx.cfg
main/midnight/ddr_v2.0_win.cfg

main/midnight/tme_history.txt
