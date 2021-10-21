#include "opengl/vbo.hpp"

#include <iostream>

constexpr GLuint noId{0};

bool isCreated(GLuint id) noexcept;

namespace OpenGL
{

VertexBufferObject::VertexBufferObject(
    VertexBufferObject::Type type, 
    VertexBufferObject::UsagePattern usagePattern)
    : id_{noId}, type_{type}, usagePattern_{usagePattern}
{
    glGenBuffers(1, &id_);

    if (!isCreated(id_))
    {
        std::cerr << "VertexBufferObject failed to instantiate." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void VertexBufferObject::allocateBufferData(const void *data, GLsizeiptr size) noexcept
{
    glBufferData(type_, size, data, usagePattern_);
}

void VertexBufferObject::bind() noexcept
{
    glBindBuffer(type_, id_);
}

void VertexBufferObject::release() noexcept
{
    glBindBuffer(type_, noId);
}

} // namespace OpenGL

inline bool isCreated(GLuint id) noexcept { return static_cast<bool>(id); }
