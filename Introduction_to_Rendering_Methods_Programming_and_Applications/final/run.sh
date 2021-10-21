# do cmake at ./build before running this script.
SCENE='resources/scene.txt'
cp -r ./resources ./build/bin/
cp -r ./resources ./build/bin/Debug
cp -r ./src/shader ./build/bin/
cp -r ./src/shader ./build/bin/Debug
cd build &&\
    make -j &&\
    cd bin/Debug &&\
    ./SampleCode $SCENE
