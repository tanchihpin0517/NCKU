#ifndef OPENGL_MESH_HPP
#define OPENGL_MESH_HPP

#include "opengl/shader.hpp"
#include "opengl/texture.hpp"
#include "opengl/vao.hpp"
#include "opengl/vbo.hpp"

#include <vector>

namespace OpenGL
{

class Mesh
{
public:
    using IndexType = unsigned int;

    Mesh(const std::vector<float> &positions,
         const std::vector<float> &normals,
         const std::vector<float> &textureCoordinates,
         const std::vector<IndexType> &indices);
    void bind();
    void release();
private:
    static void vertexBufferObjectSetup(VertexBufferObject &object,
                                        const std::vector<float> &data,
                                        GLuint index, GLint size, GLenum type,
                                        GLboolean normalized, GLsizei stride,
                                        int offset);

    std::unique_ptr<VertexArrayObject> vertexArrayObject_;
    std::array<std::unique_ptr<VertexBufferObject>, 3> vertexBufferObject_;
    std::unique_ptr<VertexBufferObject> elementBufferObject_;

    GLsizei indicesCount_;
};

}

#endif // OPENGL_MESH_HPP