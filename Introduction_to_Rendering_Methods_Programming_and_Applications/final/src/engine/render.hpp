#ifndef ENGINE_RENDER_HPP
#define ENGINE_RENDER_HPP

#include "opengl/window.hpp"
#include "opengl/shader.hpp"
#include "scene/scene.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <array>
#include <memory>
#include <string>

namespace Engine
{

class RenderModule
{
public:
    struct Light
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
    };

    RenderModule(std::array<int, 2> &openglVersion,
                 std::array<int, 2> &windowSize,
                 std::string &windowTitle);
    void loop(std::shared_ptr<Scene::Scene> &scene);
private:
    bool initializeContext(std::array<int, 2> &openglVersion,
                           std::array<int, 2> &windowSize,
                           std::string &windowTitle);

    std::unique_ptr<OpenGL::Window> window_;
    std::unique_ptr<OpenGL::Shader> shader_;
    std::unique_ptr<OpenGL::Shader> depthShader_;
    std::unique_ptr<OpenGL::Shader> debugShader_;

    std::vector<Light> lights_;

    unsigned int depthMapFBO_;
    unsigned int depthMap_;
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    float near_plane_ = 0.1f;
    float far_plane_ = 100.0f;
};

}

#endif // ENGINE_RENDER_HPP
