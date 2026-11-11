# Building on Linux

Tested on Debian 13 (x86_64) with GCC 14 and axmol 2.11.5.

## Install the tools

```
sudo apt-get install git cmake ninja-build g++ nasm pkg-config
```

- Install PowerShell 7 (axmol's setup and its CMake modules use `pwsh`): https://learn.microsoft.com/powershell/scripting/install/installing-powershell-on-linux

## Install axmol

```
git clone -b v2.11.5 https://github.com/axmolengine/axmol.git
cd axmol
pwsh ./setup.ps1
export AX_ROOT=$PWD
```

`setup.ps1` asks to install axmol's Linux packages (X11, GTK, OpenGL, ALSA, VLC and others). Answer `y`.

## Build a game

From within 'The-Lords-Of-Midnight/main/midnight'. `TME` is `LOM`, `DDR` or `CITADEL`; the target is `midnight`, `revenge` or `citadel`.

```
cmake -S . -B Builds/linux-midnight -G Ninja -DTME=LOM -DCMAKE_BUILD_TYPE=Release
cmake --build Builds/linux-midnight --target midnight
```

The game and its `Content` folder are in `Builds/linux-midnight/bin/midnight`:

```
Builds/linux-midnight/bin/midnight/midnight
```

## Build and run the unit tests

```
cmake -S . -B Builds/linux-tests-midnight -G Ninja -DTME=LOM -DTESTS=YES -DCMAKE_BUILD_TYPE=Release
cmake --build Builds/linux-tests-midnight --target midnight
Builds/linux-tests-midnight/bin/midnight/midnight --reporter compact
```

## If the window stays blank in a virtual machine

Run with Mesa's software renderer:

```
LIBGL_ALWAYS_SOFTWARE=1 Builds/linux-midnight/bin/midnight/midnight
```
