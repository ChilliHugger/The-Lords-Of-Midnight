# Building on Windows



## For both Lords Of Midnight and Doomdark's Revenge

### Install CMake

- Download CMake installer (3.31+) from: https://cmake.org/download/
- Install making sure CMake is added to your path during installation.

### Install Visual Studio 22

- Download Visual Studio 22 community edition from: https://visualstudio.microsoft.com/
- Install making sure that you select "Game development with C++" from Workloads tab, "Microsoft Visual Studio Installer Projects" from Individual Components tab.

### Install axmol

- Follow the axmol install instructions from:https://github.com/axmolengine/axmol/blob/dev/docs/DevSetup.md#prerequisites

### For Powershell 5.X

- Run powershell then do:

  Set-ExecutionPolicy  -ExecutionPolicy Bypass -Force



## For Lords of Midnight

### Create Windows VS Project

- From within 'The-Lords-Of-Midnight':

```
cd main\midnight\build.win32\midnight
mkdir vs_project && cd vs_project
cmake ..\..\.. -G"Visual Studio 17 2022" -A Win32 -DTME:string=LOM
```

### Build Code from Windows VS

- Open Visual Studio solution 'midnight.sln' from within 'main\midnight\build.win32\midnight\vs_project'
- Change target to 'Release':

![Change target to Release](images/build_windows_release.png)

- From Solution Explorer expand 'CMakePredefinedTargets', expand 'ALL_BUILD', right click on 'CMakeLists.txt' and select 'Properties':

  ![Edit ALL_BUILD CMakelist.txt](images/build_windows_all_build_cmakelists.png)

- From 'Properties' dialog, select 'Custom Build Tool\General', edit 'Command Line':

  ![Edit Command Line](images/build_windows_all_build_cmakelists_edit.png)

- Add '-DTME=LOM' to 'Command Line':

  ![Add TME property](images/build_windows_all_build_cmakelists_edit_tme.png)

- From Solution Explorer expand 'CMakePredefinedTargets', right click on 'ALL_BUILD' and select 'Build'. Wait for build to complete.

![Build 'ALL_BUILD'](images/build_windows_all_build.png)

- From Solution Explorer, right click on 'midnight' and select 'Set as Startup Project'.

- Expand 'midnight', right click on 'CMakeLists.txt', go through same process as before to add '-DTME=LOM' to 'Command Line'.

- From Solution Explorer, right click on 'midnight' and select 'Build'.

- You should now have an executable under 'vs_project\bin\midnight\Release'

- Setup working directory for debug by selecting 'Debug', then selecting 'midnight Debug Properties', then 'Debugging'. Click on 'Working Directory', select browse and choose 'bin\midnight\Debug\Content' 

- If needing to debug code, change target to 'Debug', click on 'Local Windows Debugger':

  ![Debugging](images/build_windows_debug.png)



## Create Windows Installer

See [Windows installers](#windows-installers) below.

## For Doomdark's Revenge

### Create Windows VS Project

- From within 'The-Lords-Of-Midnight':

```
cd main\midnight\build.win32\revenge
mkdir vs_project && cd vs_project
cmake ..\..\.. -G"Visual Studio 17 2022" -A Win32 -DTME:string=DDR
```

### Build Code from Windows VS

Instructions very similar to building Lords of Midnight, instead of 'midnight' you will see a 'revenge' solution. Refer to images in instructions on building Lords of Midnight for details.

- Open Visual Studio solution 'midnight.sln' from within 'main\midnight\build.win32\revenge\vs_project'
- Change target to 'Release':
- From Solution Explorer expand 'CMakePredefinedTargets', expand 'ALL_BUILD', right click on 'CMakeLists.txt' and select 'Properties'.
- From 'Properties' dialog, select 'Custom Build Tool\General', edit 'Command Line'.
- Add '-DTME=DDR' to 'Command Line'.
- From Solution Explorer expand 'CMakePredefinedTargets', right click on 'ALL_BUILD' and select 'Build'. Wait for build to complete.
- From Solution Explorer, right click on 'revenge' and select 'Set as Startup Project'.
- Expand 'revenge', right click on 'CMakeLists.txt', go through same process as before to add '-DTME=DDR' to 'Command Line'.
- From Solution Explorer, right click on 'revenge' and select 'Build'.
- You should now have an executable under 'vs_project\bin\revenge\Release'
- Setup working directory for debug by selecting 'Debug', then selecting 'revenge Debug Properties', then 'Debugging'. Click on 'Working Directory', select browse and choose 'bin\revenge\Debug\Content' 
- If needing to debug code, change target to 'Debug', click on 'Local Windows Debugger'.


## For The Citadel

### Create Windows VS Project

- From within 'The-Lords-Of-Midnight':

```
cd main\midnight\build.win32\citadel
mkdir vs_project && cd vs_project
cmake ..\..\.. -G"Visual Studio 17 2022" -A Win32 -DTME:string=CITADEL
```

### Build Code from Windows VS

Instructions very similar to building Lords of Midnight, instead of 'midnight' you will see a 'citadel' solution. Refer to images in instructions on building Lords of Midnight for details.

- Open Visual Studio solution 'midnight.sln' from within 'main\midnight\build.win32\citadel\vs_project'
- Change target to 'Release':
- From Solution Explorer expand 'CMakePredefinedTargets', expand 'ALL_BUILD', right click on 'CMakeLists.txt' and select 'Properties'.
- From 'Properties' dialog, select 'Custom Build Tool\General', edit 'Command Line'.
- Add '-DTME=DDR' to 'Command Line'.
- From Solution Explorer expand 'CMakePredefinedTargets', right click on 'ALL_BUILD' and select 'Build'. Wait for build to complete.
- From Solution Explorer, right click on 'citadel' and select 'Set as Startup Project'.
- Expand 'citadel', right click on 'CMakeLists.txt', go through same process as before to add '-DTME=CITADEL' to 'Command Line'.
- From Solution Explorer, right click on 'citadel' and select 'Build'.
- You should now have an executable under 'vs_project\bin\citadel\Release'
- Setup working directory for debug by selecting 'Debug', then selecting 'citadel Debug Properties', then 'Debugging'. Click on 'Working Directory', select browse and choose 'bin\citadel\Debug\Content' 
- If needing to debug code, change target to 'Debug', click on 'Local Windows Debugger'.


## Version Upgrade

Game versions follow the format Major.Minor.Maintenance.Build (e.g. 2.0.4.43). Edit `main\midnight\VERSION`
and run `scripts/set_version.sh` (Git Bash); it updates `game.rc` and every other platform's version.

## Windows installers

Installers are built with [Inno Setup 6](https://jrsoftware.org/isinfo.php) from one script,
`main\midnight\installer\game.iss`, for all three games (the old Visual Studio `.vdproj` projects are retired).

- Build a Release version of the game first, then from `main\midnight`:

```
scripts\build_installer.ps1 -App midnight   # or revenge / citadel
```

- The installer is written to `main\midnight\installer\Output`. Its version comes from `VERSION`.
- It installs to `Program Files (x86)\Chilli Hugger Software Ltd\<game>` and, on first run, uninstalls an
  MSI-installed older version (found through the old UpgradeCode) so users upgrade in place.
- Never change the `AppId` values in `game.iss`; they are how later installers upgrade earlier ones.
- CI does all of this: the "Windows build" workflow builds, tests and packages every game on each push to main.

## About icons

To refresh icon cache (in case you do not see an updated icon for your final executable) do:

```
ie4uinit.exe -ClearIconCache
```

On Windows 10 do instead:

```
ie4uinit.exe -show
```