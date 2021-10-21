#ifndef OPENGL_VAO_HPP
#define OPENGL_VAO_HPP

#include "glad/glad.h"

#include <memory>

namespace OpenGL
{

class VertexArrayObject
{
public:
    VertexArrayObject();
    void bind();
    void release();
private:
    GLuint id_;
};

}

#endif // OPENGL_VAO_HPP