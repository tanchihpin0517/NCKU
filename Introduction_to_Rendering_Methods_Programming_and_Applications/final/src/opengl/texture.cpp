#include "opengl/texture.hpp"

#include "stb_image.h"

#include <iostream>

bool isCreated(GLuint id) noexcept;
GLenum rgbFormat(int channels) noexcept;

constexpr GLuint noId{0};

namespace OpenGL
{

Texture::Texture(const char *textureFile)
    : id_{noId}, format_{0}, height_{0}, width_{0}, mipmapCount_{0},
      minificationFilter_{Filter::Nearest},
      magnificationFilter_{Filter::Linear}, wrapOption_{WrapOption::Repeat}
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data{stbi_load(textureFile, &width_, &height_, &channels_, 0)};

    if (!data)
    {
        std::cerr << "Failed to load texture file: " << textureFile << std::endl;
        exit(EXIT_FAILURE);
    }

    const size_t size{static_cast<size_t>(width_ * height_ * channels_)};

    std::vector<unsigned char> buffer{data, data + size};
    stbi_image_free(data);

    format_ = rgbFormat(channels_);
    create();
    
    bindBuffer(buffer);
}

void Texture::create()
{
    glGenTextures(1, &id_);

    if (!isCreated(id_))
    {
        std::cerr << "Failed to create texture" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Texture::bind()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id_);
}

void Texture::release()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noId);
}

void Texture::bindBuffer(const std::vector<unsigned char> &buffer)
{
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minificationFilter_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, magnificationFilter_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapOption_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapOption_);

    glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, format_,
                 GL_UNSIGNED_BYTE, buffer.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    release();
}

} // namespace OpenGL

inline bool isCreated(GLuint id) noexcept { return static_cast<bool>(id); }

GLenum rgbFormat(int channels) noexcept
{
    switch (channels)
    {
    case 1:
        return GL_RED;
    case 2:
        return GL_RG;
    case 4:
        return GL_RGBA;
    case 3:
    default:
        return GL_RGB;
    }
}