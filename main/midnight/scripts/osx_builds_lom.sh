find . -type f -name '.DS*' -delete

rm -r Builds/mac-lom
mkdir Builds/mac-lom
axmol build -configOnly -p osx -a x64 -xc '-DTME=LOM,-BBuilds/mac-lom'
#axmol build -configOnly -p osx -xc '-DTME=LOM,-BBuilds/mac-lom,-DCMAKE_OSX_ARCHITECTURES=$(ARCHS_STANDARD)'

rm -r Builds/mac-tests-lom
mkdir Builds/mac-tests-lom
axmol build -configOnly -p osx -a x64 -xc '-DTME=LOM,-DTESTS=YES,-BBuilds/mac-tests-lom'

rm -r Builds/ios-lom
mkdir Builds/ios-lom
axmol build -configOnly -p ios -a arm64 -xc '-DTME=LOM,-BBuilds/ios-lom'

rm -r Builds/ios-lom-simulator
mkdir Builds/ios-lom-simulator
axmol build -configOnly -p ios -a x64 -sdk simulator -xc '-DTME=LOM,-BBuilds/ios-lom-simulator'
