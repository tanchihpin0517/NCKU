#ifndef OPENGL_SHADER_HPP
#define OPENGL_SHADER_HPP

#include "glad/glad.h"
#include "glm/glm.hpp"

#include <string>

namespace OpenGL
{

class Shader
{
public:
    Shader(const char *vertexShaderSource,
           const char *fragmentShaderSource,
           const char *geometryShaderSource = nullptr);
    void use() noexcept;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setInt(const std::string &name, const int &value);
    void setFloat(const std::string &name, float value);
private:
    void createProgram();
    void makeFromFile(GLuint &id, GLenum type, const char *fileName);
    void link();
    bool linkStatus();

    GLuint programId_;
    GLuint vShaderId_;
    GLuint fShaderId_;
    GLuint gShaderId_;
};

}

#endif // OPENGL_SHADER_HPP
