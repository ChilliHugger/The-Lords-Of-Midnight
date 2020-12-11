# The Lords of Midnight
All data regarding The Lords of Midnight
Including Doomdark's Revenge and The Citadel

# Building on OSX

## Install CMake
https://cmake.org/download/

## Install Cocos2d-x
https://github.com/cocos2d/cocos2d-x
https://docs.cocos.com/cocos2d-x/manual/en/installation/

## Create Projects
https://docs.cocos.com/cocos2d-x/manual/en/installation/CMake-Guide.html

## Create OSX XCode Project
```
cd main/midnight
mkdir mac-build
cd mac-build
cmake .. -GXcode
```

## Create iOS XCode Project
cd main/midnight
mkdir ios-build
cd ios-build
cmake .. -GXcode -DCMAKE_SYSTEM_NAME=iOS

# Building on Windows 

## For both Lords Of Midnight and Doomdark's Revenge

### Install CMake

- Download CMake installer from: https://cmake.org/download/
- Install making sure CMake is added to your path during installation.

### Install Visual Studio 19

- Download Visual Studio 19 community edition from: https://visualstudio.microsoft.com/
- Install making sure that you select "Game development with C++" from Workloads tab.

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

Start from unchanged code checkout from github

### Create Windows VS Project
- From within 'The-Lords-Of-Midnight':
```
cd main\midnight
mkdir win32-build && cd win32-build
cmake .. -G"Visual Studio 16 2019" -A Win32
```
### Build Code from Windows VS

- Open Visual Studio solution 'midnight.sln' from within win32-build
- From Solution Explorer expand 'CMakePredefinedTargets', right click on 'ALL_BUILD' and select 'Build'. Wait for build to complete.
- From Solution Explorer, right click on 'midnight' and select 'Set as Startup Project'
- From Solution Explorer, right click on 'midnight' and select 'Build'
- You should now have an executable under 'win32-build\bin\midnight\Debug'


## For Doomdark's Revenge

## Change CMakeLists to build DDR

* Edit 'main\midnight\CMakeLists.txt'
* Tweak the section defining project to build by commenting out 'LOM' section and uncommenting the 'DDR' section:
```
#if(_LOM_)
#  set(APP_BUILD _LOM_)
#  set(APP_NAME midnight)
#elseif(_DDR_)
  set(APP_BUILD _DDR_)
  set(APP_NAME revenge)
#endif()
```

### Create Windows VS Project
- From within 'The-Lords-Of-Midnight':
```
cd main\midnight
mkdir win32-build && cd win32-build
cmake .. -G"Visual Studio 16 2019" -A Win32
```
### Build Code from Windows VS

- Open Visual Studio solution 'revenge.sln' from within win32-build
- From Solution Explorer expand 'CMakePredefinedTargets', right click on 'ALL_BUILD' and select 'Build'. Wait for build to complete.
- From Solution Explorer, right click on 'revenge' and select 'Set as Startup Project'
- From Solution Explorer, right click on 'revenge' and select 'Build'
- You should now have an executable under 'win32-build\bin\midnight\Debug'

