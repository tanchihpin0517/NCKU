#ifndef SAMPLECODE_MODEL_TEXTUREFACTORY_HPP_
#define SAMPLECODE_MODEL_TEXTUREFACTORY_HPP_

#include "OpenGL/OpenGLTexture.hpp"

#include <memory>

namespace Model
{

class TextureFactory
{
public:
    static std::unique_ptr<OpenGL::OpenGLTexture>
    loadFromFile(const char *fileName);
};

} // namespace Model

#endif // SAMPLECODE_MODEL_TEXTUREFACTORY_HPP_
