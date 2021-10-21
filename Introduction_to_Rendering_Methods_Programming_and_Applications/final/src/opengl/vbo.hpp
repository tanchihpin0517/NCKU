#ifndef OPENGL_VBO_HPP
#define OPENGL_VBO_HPP

#include "glad/glad.h"

#include <memory>

namespace OpenGL
{

class VertexBufferObject
{
public:
    /**
     *  \brief This enum represents the type of OpenGLBufferObject.
     */
    enum Type
    {
        /**
         * \brief Vertex buffer object
         */
        ArrayBuffer = GL_ARRAY_BUFFER,
        /**
         * \brief Index buffer object
         */
        ElementArrayBuffer = GL_ELEMENT_ARRAY_BUFFER
    };

    /**
     *  \brief This enum represents the usage pattern of OpenGLBufferObject.
     */
    enum UsagePattern
    {
        /**
         * \brief Data will be modified once and used at most a few times for
         * drawing.
         */
        StreamDraw = GL_STREAM_DRAW,
        /**
         * \brief Data will be modified once and used at most a few times for
         * read data back to OpenGL server.
         */
        StreamRead = GL_STREAM_READ,
        /**
         * \brief Data will be modified once and used at most a few times for
         * read data back to OpenGL server and used in further drawing.
         */
        StreamCopy = GL_STREAM_COPY,
        /**
         * \brief Data will be modified once and used many times for drawing.
         */
        StaticDraw = GL_STATIC_DRAW,
        /**
         * \brief Data will be modified once and used many times for read data
         * back to OpenGL server.
         */
        StaticRead = GL_STATIC_READ,
        /**
         * \brief Data will be modified once and used many times for read data
         * back to OpenGL server and used in further drawing.
         */
        StaticCopy = GL_STATIC_COPY,
        /**
         * \brief Data will be modified many times and used many times for
         * drawing.
         */
        DynamicDraw = GL_DYNAMIC_DRAW,
        /**
         * \brief Data will be modified many times and used many times for read
         * data back to OpenGL server.
         */
        DynamicRead = GL_DYNAMIC_READ,
        /**
         * \brief Data will be modified many times and used many times for read
         * data back to OpenGL server and used in further drawing.
         */
        DynamicCopy = GL_DYNAMIC_COPY,
    };

    explicit VertexBufferObject(VertexBufferObject::Type type,
                                VertexBufferObject::UsagePattern usagePattern);
    void allocateBufferData(const void *data, GLsizeiptr size) noexcept;
    void bind() noexcept;
    void release() noexcept;

private:
    GLuint id_;
    VertexBufferObject::Type type_;
    VertexBufferObject::UsagePattern usagePattern_;

};

} // namespace OpenGL

#endif // OPENGL_VBO_HPP