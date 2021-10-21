#include "opengl/mesh.hpp"

#define PROGRAM_BUFFER_OFFSET(x) (static_cast<char *>(0) + (x))

namespace OpenGL
{

Mesh::Mesh(const std::vector<float> &positions,
           const std::vector<float> &normals,
           const std::vector<float> &textureCoordinates,
           const std::vector<IndexType> &indices)
    : vertexArrayObject_{nullptr}, vertexBufferObject_{{nullptr, nullptr,
                                                        nullptr}},
      elementBufferObject_{nullptr},
      indicesCount_{static_cast<GLsizei>(indices.size())}
{
    vertexArrayObject_.reset(new VertexArrayObject{});
    for (auto &object : vertexBufferObject_)
    {
        object.reset(new VertexBufferObject{
            VertexBufferObject::Type::ArrayBuffer,
            VertexBufferObject::UsagePattern::StaticDraw});
    }
    elementBufferObject_.reset(new VertexBufferObject{
        VertexBufferObject::Type::ElementArrayBuffer,
        VertexBufferObject::UsagePattern::StaticDraw});

    vertexArrayObject_->bind();

    vertexBufferObjectSetup(*(vertexBufferObject_[0]), positions,
                            0, 3, GL_FLOAT, GL_FALSE,
                            3 * sizeof(float), 0);

    vertexBufferObjectSetup(*(vertexBufferObject_[1]), normals,
                            1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    vertexBufferObjectSetup(*(vertexBufferObject_[2]), textureCoordinates,
                            2, 2, GL_FLOAT, GL_FALSE,
                            2 * sizeof(float), 0);

    elementBufferObject_->bind();
    elementBufferObject_->allocateBufferData(
        indices.data(), sizeof(IndexType) * indices.size());

    vertexArrayObject_->release();
}

void Mesh::vertexBufferObjectSetup(VertexBufferObject &object,
                                   const std::vector<float> &data,
                                   GLuint index, GLint size, GLenum type,
                                   GLboolean normalized, GLsizei stride,
                                   int offset)
{
    object.bind();
    object.allocateBufferData(data.data(), sizeof(float) * data.size());

    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, normalized, stride,
                          PROGRAM_BUFFER_OFFSET(offset));
}

void Mesh::bind()
{
    vertexArrayObject_->bind();
}

void Mesh::release()
{
    vertexArrayObject_->release();
}

} // namespace OpenGL