rm -r mac-build
mkdir mac-build
cd mac-build
cmake .. -GXcode -DTME:string=LOM
cd ..

rm -r mac-tests-build
mkdir mac-tests-build
cd mac-tests-build
cmake .. -GXcode -DTME:string=LOM -DTESTS=YES
cd ..

rm -r ios-build
mkdir ios-build
cd ios-build
cmake .. -GXcode -DCMAKE_SYSTEM_NAME=iOS -DTME:string=LOM
cd ..
