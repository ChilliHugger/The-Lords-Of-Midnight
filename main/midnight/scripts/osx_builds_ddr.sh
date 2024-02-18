find . -type f -name '.DS*' -delete 

rm -r mac-build-ddr
mkdir mac-build-ddr
cd mac-build-ddr
cmake .. -GXcode -DTME:string=DDR
attr -w com.apple.xcode.CreatedByBuildSystem true build
cd ..

rm -r mac-tests-build-ddr
mkdir mac-tests-build-ddr
cd mac-tests-build-ddr
cmake .. -GXcode -DTME:string=DDR -DTESTS=YES
attr -w com.apple.xcode.CreatedByBuildSystem true build
cd ..

rm -r ios-build-ddr
mkdir ios-build-ddr
cd ios-build-ddr
cmake .. -GXcode -DCMAKE_SYSTEM_NAME=iOS -DTME:string=DDR
attr -w com.apple.xcode.CreatedByBuildSystem true build
cd ..
