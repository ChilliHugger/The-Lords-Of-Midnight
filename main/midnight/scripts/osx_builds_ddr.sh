find . -type f -name '.DS*' -delete

rm -r Builds/mac-ddr
mkdir Builds/mac-ddr
axmol build -configOnly -p osx -a x64 -xc '-DTME=DDR,-BBuilds/mac-ddr'

rm -r Builds/mac-tests-ddr
mkdir Builds/mac-tests-ddr
axmol build -configOnly -p osx -a x64 -xc '-DTME=DDR,-DTESTS=YES,-BBuilds/mac-tests-ddr'

rm -r Builds/ios-ddr
mkdir Builds/ios-ddr
axmol build -configOnly -p ios -a arm64 -xc '-DTME=DDR,-BBuilds/ios-ddr'

# TVOS Simulator:
rm -r Builds/tvos-ddr-simulator
mkdir Builds/tvos-ddr-simulator
axmol build -configOnly -p tvos -a x64 -sdk simulator -xc '-DTME=DDR,-BBuilds/tvos-ddr-simulator'

# TVOS Device:
rm -r Builds/tvos-ddr
mkdir Builds/tvos-ddr
axmol build -configOnly -p tvos -a arm64 -xc '-DTME=DDR,-BBuilds/tvos-ddr'
