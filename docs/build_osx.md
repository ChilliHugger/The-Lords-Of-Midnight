# Building on OSX

[TOC]

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
cmake .. -GXcode -DTME:string=LOM
```

Replace LOM with DDR for Doomdark's Revenge

## Create iOS XCode Project

```
cd main/midnight
mkdir ios-build
cd ios-build
cmake .. -GXcode -DCMAKE_SYSTEM_NAME=iOS -DTME:string=LOM
```

Replace LOM with DDR for Doomdark's Revenge