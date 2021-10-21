# do cmake at ./build before running this script.
SCENE='resources/scene.txt'
cp -r ./resources ./build/bin/Debug
cp -r ./src/shader ./build/bin/
cd build &&\
    make -j &&\
    cd bin/Debug &&\
    gdb -q ./SampleCode $SCENE
