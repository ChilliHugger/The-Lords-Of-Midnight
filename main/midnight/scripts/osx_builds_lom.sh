find . -type f -name '.DS*' -delete

rm -r Builds/mac-lom
mkdir Builds/mac-lom
axmol build -configOnly -p osx -a x64 -xc '-DTME=LOM,-BBuilds/mac-lom'

rm -r Builds/mac-tests-lom
mkdir Builds/mac-tests-lom
axmol build -configOnly -p osx -a x64 -xc '-DTME=LOM,-DTESTS=YES,-BBuilds/mac-tests-lom'

rm -r Builds/ios-lom
mkdir Builds/ios-lom
axmol build -configOnly -p ios -a arm64 -xc '-DTME=LOM,-BBuilds/ios-lom'
