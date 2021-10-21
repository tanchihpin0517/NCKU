#include "opengl/window.hpp"

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

namespace OpenGL
{

void frameBufferSizeCallback(GLFWwindow *window, int width, int height);

void frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    if (window) {}
    glViewport(0, 0, width, height);
}

Window* Window::create(std::array<int, 2> &size, std::string &title)
{
    Window* window = new Window(size, title);

    if (!window->glwindow_)
    {
        return nullptr;
    }

    glfwMakeContextCurrent(window->glwindow_);
    glfwSetFramebufferSizeCallback(window->glwindow_, frameBufferSizeCallback);
    // glfwSwapInterval(0);
    return window;
}

Window::Window(std::array<int, 2> &size, std::string &title)
    : glwindow_{nullptr}, size_{size}, title_{title}
{
    glwindow_ =
        glfwCreateWindow(size_[0], size_[1], title_.c_str(),
                         nullptr, nullptr);
}

bool Window::updateFrame()
{
    if (!(glfwGetKey(glwindow_, GLFW_KEY_ESCAPE) == GLFW_PRESS))
    {
        glfwSwapBuffers(glwindow_);
        glfwPollEvents();
        return true;
    }
    else
    {
        return false;
    }
}

float Window::aspectRatio() const noexcept
{
    return static_cast<float>(width()) / static_cast<float>(height());
}

int Window::width() const { return size_[0]; }

int Window::height() const { return size_[1]; }

}
