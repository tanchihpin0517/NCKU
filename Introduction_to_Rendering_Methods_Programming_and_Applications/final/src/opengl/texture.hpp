#ifndef OPENGL_TEXTURE_HPP
#define OPENGL_TEXTURE_HPP

#include "glad/glad.h"

#include <memory>
#include <vector>

namespace OpenGL
{

class Texture
{
public:
    enum Filter
    {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,
        NearestMipMapNearest = GL_NEAREST_MIPMAP_NEAREST,
        LinearMipMapNearest = GL_LINEAR_MIPMAP_NEAREST,
        NearestMipMapLinear = GL_NEAREST_MIPMAP_LINEAR,
        LinearMipMapLinear = GL_LINEAR_MIPMAP_LINEAR
    };

    enum WrapOption
    {
        Repeat = GL_REPEAT,
        MirroredRepeat = GL_MIRRORED_REPEAT,
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampTOBorder = GL_CLAMP_TO_BORDER
    };

    Texture(const char *textureFile);
    void bind();
    void release();
private:
    void create();
    void bindBuffer(const std::vector<unsigned char> &buffer);

    GLuint id_;

    GLenum format_;
    GLsizei height_;
    GLsizei width_;
    GLsizei channels_;

    GLuint mipmapCount_;

    Filter minificationFilter_;
    Filter magnificationFilter_;
    WrapOption wrapOption_;
};

} // namespace OpenGL

#endif // OPENGL_TEXTURE_HPP