find . -type f -name '.DS*' -delete

# OSX
rm -r Builds/mac-lom
mkdir Builds/mac-lom
axmol build -configOnly -p osx -xc '-DTME=LOM,-BBuilds/mac-lom,-DCMAKE_OSX_ARCHITECTURES=$(ARCHS_STANDARD)'

OSX Tests
rm -r Builds/mac-tests-lom
mkdir Builds/mac-tests-lom
axmol build -configOnly -p osx -a x64 -xc '-DTME=LOM,-DTESTS=YES,-BBuilds/mac-tests-lom'

iOS
rm -r Builds/ios-lom
mkdir Builds/ios-lom
axmol build -configOnly -p ios -a arm64 -xc '-DTME=LOM,-BBuilds/ios-lom'

# TVOS Simulator:
rm -r Builds/tvos-lom-simulator
mkdir Builds/tvos-lom-simulator
axmol build -configOnly -p tvos -a x64 -sdk simulator -xc '-DTME=LOM,-BBuilds/tvos-lom-simulator'

# TVOS Device:
rm -r Builds/tvos-lom
mkdir Builds/tvos-lom
axmol build -configOnly -p tvos -a arm64 -xc '-DTME=LOM,-BBuilds/tvos-lom'
