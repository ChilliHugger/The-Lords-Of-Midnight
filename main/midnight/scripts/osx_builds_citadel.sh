find . -type f -name '.DS*' -delete

rm -r Builds/mac-citadel
mkdir Builds/mac-citadel
axmol build -configOnly -p osx -a x64 -xc '-DTME=CITADEL,-BBuilds/mac-citadel'

rm -r Builds/mac-tests-citadel
mkdir Builds/mac-tests-citadel
axmol build -configOnly -p osx -a x64 -xc '-DTME=CITADEL,-DTESTS=YES,-BBuilds/mac-tests-citadel'

rm -r Builds/ios-citadel
mkdir Builds/ios-citadel
axmol build -configOnly -p ios -a arm64 -xc '-DTME=CITADEL,-BBuilds/ios-citadel'

rm -r Builds/ios-citadel-simulator
mkdir Builds/ios-citadel-simulator
axmol build -configOnly -p ios -a x64 -sdk simulator -xc '-DTME=CITADEL,-BBuilds/ios-citadel-simulator'
