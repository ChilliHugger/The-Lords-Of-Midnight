# Building on Windows



## For both Lords Of Midnight and Doomdark's Revenge

### Install CMake

- Download CMake installer from: https://cmake.org/download/
- Install making sure CMake is added to your path during installation.

### Install Visual Studio 19

- Download Visual Studio 19 community edition from: https://visualstudio.microsoft.com/
- Install making sure that you select "Game development with C++" and "Microsoft Visual Studio Installer Projects" from Workloads tab.

### Install coco2d

- Download and install Python 2.7 from: https://www.python.org/downloads/windows/
- Download cocos2d-x version 4.0 from: http://www.cocos2d-x.org
- Unzip coco2d-x into location where you want it (e.g. C:\opt\coco2d-x-4.0)
- From command line run setup.py from within unzipped directory
- Installer should have setup environment variables:
  - COCOS_CONSOLE_ROOT
  - COCOS_X_ROOT
  - COCOS_TEMPLATES_ROOT

### Generate coco2d code

- Create a temporary directory (e.g. 'C:\coco2d-tmp')
- Run

```
  cocos new codebuild -p chilli.codebuild -l cpp -d .
```

- Copy directory 'codebuild\cocos2d' into 'The-Lords-Of-Midnight\main\midnight'

## For Lords of Midnight

### Create Windows VS Project

- From within 'The-Lords-Of-Midnight':

```
cd main\midnight\build.win32\midnight
mkdir vs_project && cd vs_project
cmake ..\..\.. -G"Visual Studio 16 2019" -A Win32 -DTME:string=LOM
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

- If needing to debug code, change target to 'Debug', click on 'Local Windows Debugger':

  ![Debugging](images/build_windows_debug.png)



## Create Windows Installer

A pre-defined project is built already present to support building installer from executable created by the Windows Visual Studio project:

- Open Visual studio.
- Open solution 'lords_of_midnight.sln' from 'main\midnight\build.win32\midnight\installer'.
- Change target to 'Release', from 'Solution Explorer' right-click on 'lords_of_midnight' and click on 'Build':

![Create installer](images/installer_windows_build.png)

- Your installer can now be found under 'main\midnight\build.win32\midnight\installer\Release\'

## For Doomdark's Revenge

### Create Windows VS Project

- From within 'The-Lords-Of-Midnight':

```
cd main\midnight\build.win32\revenge
mkdir vs_project && cd vs_project
cmake ..\..\.. -G"Visual Studio 16 2019" -A Win32 -DTME:string=DDR
```

### Build Code from Windows VS

Instructions very similar to building Lords of Midnight, instead of 'midnight' you will see a 'revenge' solution. Refer to images in instructions on building Lords of Midnight for details.

- Open Visual Studio solution 'midnight.sln' from within 'main\midnight\build.win32\revenge\vs_project'

- Change target to 'Release':

- From Solution Explorer expand 'CMakePredefinedTargets', expand 'ALL_BUILD', right click on 'CMakeLists.txt' and select 'Properties'.
- From 'Properties' dialog, select 'Custom Build Tool\General', edit 'Command Line'.
- From 'Properties' dialog, select 'Custom Build Tool\General', edit 'Command Line'.
- Add '-DTME=DDR' to 'Command Line'.
- From Solution Explorer expand 'CMakePredefinedTargets', right click on 'ALL_BUILD' and select 'Build'. Wait for build to complete.
- From Solution Explorer, right click on 'revenge' and select 'Set as Startup Project'.
- Expand 'revenge', right click on 'CMakeLists.txt', go through same process as before to add '-DTME=DDR' to 'Command Line'.
- From Solution Explorer, right click on 'revenge' and select 'Build'.
- You should now have an executable under 'vs_project\bin\revenge\Release'
- If needing to debug code, change target to 'Debug', click on 'Local Windows Debugger'.

## Create Windows Installer

A pre-defined project is built already present to support building installer from executable created by the Windows Visual Studio project:

- Open Visual studio.
- Open solution 'doomdarks_revenge.sln' from 'main\midnight\build.win32\revenge\installer'.
- Change target to 'Release', from 'Solution Explorer' right-click on 'lords_of_midnight' and click on 'Build'.
- Your installer can now be found under 'main\midnight\build.win32\revenge\installer\Release\'

## Version Upgrade

Game versions follow the format Major.Minor.Maintenance.Build (e.g. 2.0.4.43)

### In Build Project

To upgrade version do the following:

- For build, edit game.rc (for Lords of Midnight file under The-Lords-Of-Midnight\main\midnight\proj.win32\midnight, for Doomdark's Revenge file under The-Lords-Of-Midnight\main\midnight\proj.win32\revenge). Change version 'FILEVERSION' under 'Version' section:

![build_windows_version_build](images/build_windows_version_build.png)

- Rebuild your project

### In Installer Project

To upgrade version do the following:

- In Solution Explorer, look at 'Deployment Project Properties', scroll down until you see the version property, change it to reflect 'Major.Minor.Maintenance' (e.g. 2.0.4):

![build_windows_version_installer](images/build_windows_version_installer.png)

- Once prompted to change 'ProductCode' click on 'No': 

![build_windows_version_installer_confirm](images/build_windows_version_installer_confirm.PNG)

## About icons

To refresh icon cache (in case you do not see an updated icon for your final executable) do:

```
ie4uinit.exe -ClearIconCache
```

On Windows 10 do instead:

```
ie4uinit.exe -show
```