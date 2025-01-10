# Building on OSX

## Install CMake

https://cmake.org/download/

### Install axmol

- Follow the axmol install instructions from: https://github.com/axmolengine/axmol/blob/dev/docs/DevSetup.md

## Create Projects

https://docs.cocos.com/cocos2d-x/manual/en/installation/CMake-Guide.html

## Create OSX XCode Project

```
cd main/midnight/builds
mkdir mac-lom
cd mac-lom
axmol build -configOnly -p osx -a x64 -xc '-DTME=LOM,-BBuilds/mac-lom'
```

## Create OSX XCode Project for Tests

```
cd main/midnight/builds
mkdir mac-tests-lom
cd mac-tests-lom
axmol build -configOnly -p osx -a x64 -xc '-DTME=LOM,-DTESTS=YES,-BBuilds/mac-tests-lom'
```

Replace LOM with DDR for Doomdark's Revenge

## Create iOS XCode Project

```
cd main/midnight/builds
mkdir ios-lom
cd ios-lom
axmol build -configOnly -p ios -a arm64 -xc '-DTME=LOM,-BBuilds/ios-lom'
```

Replace LOM with DDR for Doomdark's Revenge
