cmake_minimum_required(VERSION 3.3.0)

set(${PROJECT_NAME}_EXECUTABLE_NAME ${PROJECT_NAME})

include(${${PROJECT_NAME}_MODULE_DIR}/CompilerOptions.cmake)

set(${PROJECT_NAME}_HEADER_CODE
    engine/engine.hpp
    engine/render.hpp
    engine/physics.hpp
    engine/illumination.hpp
    scene/scene.hpp
    scene/environment.hpp
    scene/object.hpp
    scene/camera.hpp
    opengl/shader.hpp
    opengl/window.hpp
    opengl/texture.hpp
    opengl/mesh.hpp
    opengl/vao.hpp
    opengl/vbo.hpp
)

set(${PROJECT_NAME}_INLINE_CODE
)

set(${PROJECT_NAME}_SOURCE_CODE
    main.cpp
    engine/engine.cpp
    engine/render.cpp
    engine/physics.cpp
    engine/illumination.cpp
    scene/scene.cpp
    scene/environment.cpp
    scene/object.cpp
    scene/camera.cpp
    opengl/shader.cpp
    opengl/window.cpp
    opengl/texture.cpp
    opengl/mesh.cpp
    opengl/vao.cpp
    opengl/vbo.cpp
)

add_executable(${${PROJECT_NAME}_EXECUTABLE_NAME}
    ${${PROJECT_NAME}_HEADER_CODE}
    ${${PROJECT_NAME}_INLINE_CODE}
    ${${PROJECT_NAME}_SOURCE_CODE}
)

set_target_properties(${${PROJECT_NAME}_EXECUTABLE_NAME}
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>
)

target_include_directories(${${PROJECT_NAME}_EXECUTABLE_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${OPENGL_INCLUDE_DIR}
        ${GLM_INCLUDE_DIRS}
        ${IMGUI_INCLUDE_DIRS}
        ${TINYOBJLOADER_INCLUDE_DIRS}
        ${STB_INCLUDE_DIRS}
)

target_compile_features(${${PROJECT_NAME}_EXECUTABLE_NAME}
    PUBLIC
        cxx_std_11
)

target_compile_options(${${PROJECT_NAME}_EXECUTABLE_NAME}
    PUBLIC
        "$<$<CONFIG:DEBUG>:${${PROJECT_NAME}_CXX_FLAGS_DEBUG}>"
        "$<$<CONFIG:RELEASE>:${${PROJECT_NAME}_CXX_FLAGS_RELEASE}>"
)

target_compile_definitions(${${PROJECT_NAME}_EXECUTABLE_NAME}
    PRIVATE
        GLM_FORCE_SILENT_WARNINGS
)

target_link_libraries(${${PROJECT_NAME}_EXECUTABLE_NAME}
    PRIVATE
        ${OPENGL_gl_LIBRARY}
        glad
        glfw
        imgui
        stb
        tinyobjloader
        Threads::Threads
        $<$<PLATFORM_ID:Linux>:${CMAKE_DL_LIBS}>
)

include(${${PROJECT_NAME}_MODULE_DIR}/PostBuildCommand.cmake)
