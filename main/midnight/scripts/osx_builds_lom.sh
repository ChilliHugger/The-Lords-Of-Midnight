find . -type f -name '.DS*' -delete

rm -r Builds/mac-build-lom
mkdir Builds/mac-build-lom
#cmake -GXcode -DTME:string=LOM -B Builds/mac-build-lom
axmol build -configOnly -p osx -a x64 -xc '-DTME=LOM,-BBuilds/mac-build-lom'
#attr -w com.apple.xcode.CreatedByBuildSystem true build

#rm -r mac-tests-build-lom
#mkdir mac-tests-build-lom
#cd mac-tests-build-lom
#cmake .. -GXcode -DTME:string=LOM -DTESTS=YES
#attr -w com.apple.xcode.CreatedByBuildSystem true build
#cd ..

#rm -r ios-build-lom
#mkdir ios-build-lom
#cd ios-build-lom
#cmake .. -GXcode -DCMAKE_SYSTEM_NAME=iOS -DTME:string=LOM
#attr -w com.apple.xcode.CreatedByBuildSystem true build
#cd ..
