mkdir -p build
cd build && cmake .. && \
make -j && echo "" && \
cd bin && \
./Homework01 resources/model/Utah_teapot_\(solid\)_texture.obj  resources/texture/uv.png Shader/BasicVertexShader.vs.glsl Shader/BasicFragmentShader.fs.glsl
