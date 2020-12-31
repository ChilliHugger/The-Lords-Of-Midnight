### Deployment Process

## iOS
1. Make sure there is no LOM/DDR folder in resources (after Android build)
2. Update Bundle Version in info.plist
3. Generate Project - 'cmake .. -GXcode -DCMAKE_SYSTEM_NAME=iOS -DTME:string=LOM'
4. Update Archive Name
  - Product | Scheme | Edit Scheme | Archive | Archive Name - "The Lords of Midnight" | "Doomdark's Revenge"
5. Check Build Version
  - Project 'Midnight' | General | Build
6. Build / Archive
7. Validate
8. Distribute
  - App Store Connect
  - Upload
  - Upload Symbols
  - Automatically Manage Signing

## OS X
1. Make sure there is no LOM/DDR folder in resources (after Android build)
2. Update Bundle Version in info.plist
3. Generate Project - 'cmake .. -GXcode -DTME:string=LOM'
4. Update Archive Name
  - Product | Scheme | Edit Scheme | Archive | Archive Name - "The Lords of Midnight" | "Doomdark's Revenge"
5. Check Build Version
  - Project 'Midnight' | General | Build
6. Check Deployment Target - 10.9
7. Check Executable File '${EXECUTABLE_NAME}'
  - Project 'Midnight' | Info | Executable file
8. Check SDK - 'MacOS'
  - Project 'Midnight' | Build Settings | Architectures | MacOS
9. Check 'Hardened Runtime'
  - Project 'Midnight' | Build Settings | Signing | Enable Hardened Runtime
10. Build / Archive
11. Validate
12. Distribute App
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
