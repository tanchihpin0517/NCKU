#ifndef OPENGL_WINDOW_HPP
#define OPENGL_WINDOW_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <memory>
#include <string>
#include <array>

namespace OpenGL
{

class Window
{
public:
    static Window* create(std::array<int, 2> &windowSize,
                          std::string &windowTitle);
    Window(std::array<int, 2> &windowSize, std::string &windowTitle);
    bool updateFrame();
    float aspectRatio() const noexcept;
    int width() const;
    int height() const;
private:
    GLFWwindow *glwindow_;

    std::array<int, 2> size_;
    std::string title_;
};

}

#endif // OPENGL_WINDOW_HPP
