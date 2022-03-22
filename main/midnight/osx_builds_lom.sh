rm -r mac-build-lom
mkdir mac-build-lom
cd mac-build-lom
cmake .. -GXcode -DTME:string=LOM
cd ..

rm -r mac-tests-build-lom
mkdir mac-tests-build-lom
cd mac-tests-build-lom
cmake .. -GXcode -DTME:string=LOM -DTESTS=YES
cd ..

rm -r ios-build-lom
mkdir ios-build-lom
cd ios-build-lom
cmake .. -GXcode -DCMAKE_SYSTEM_NAME=iOS -DTME:string=LOM
cd ..
