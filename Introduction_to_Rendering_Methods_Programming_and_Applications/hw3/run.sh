# do cmake at ./build before running this script.
MODEL='resources/model/cube.obj'
TEXTURE='resources/texture/uv.png'
VSHADER='Shader/BasicVertexShader.vs.glsl'
FSHADER='Shader/BasicFragmentShader.fs.glsl'
cd build &&\
    make -j &&\
    cd bin &&\
    ./SampleCode $MODEL $TEXTURE $VSHADER $FSHADER
