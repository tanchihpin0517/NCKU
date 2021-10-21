#include "opengl/vao.hpp"

#include <iostream>

constexpr GLuint noId{0};

bool isCreated(GLuint id) noexcept;

namespace OpenGL
{

VertexArrayObject::VertexArrayObject() : id_{noId}
{
    glGenVertexArrays(1, &id_);

    if (!isCreated(id_))
    {
        std::cerr << "VertexArrayObject failed to instantiate." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void VertexArrayObject::bind()
{
    glBindVertexArray(id_);
}

void VertexArrayObject::release()
{
    glBindVertexArray(noId);
}

}

inline bool isCreated(GLuint id) noexcept { return static_cast<bool>(id); }
