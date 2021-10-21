#include "opengl/shader.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>

std::streampos getTextLength(std::ifstream &in);
std::string readFileFullText(const char *fileName);
bool readFileFullText(const char *fileName, std::string &outputText);
bool compileStatus(GLuint id) noexcept;
bool isCreated(GLuint id) noexcept;

namespace OpenGL
{

constexpr GLuint noId{0};

Shader::Shader(const char *vertexShaderSource,
               const char *fragmentShaderSource,
               const char *geometryShaderSource)
    : programId_{noId}, vShaderId_{noId}, fShaderId_{noId}, gShaderId_{noId}
{
    createProgram();

    if (vertexShaderSource)
    {
        makeFromFile(vShaderId_, GL_VERTEX_SHADER, vertexShaderSource);
        glAttachShader(programId_, vShaderId_);
    }

    if (fragmentShaderSource)
    {
        makeFromFile(fShaderId_, GL_FRAGMENT_SHADER, fragmentShaderSource);
        glAttachShader(programId_, fShaderId_);
    }

    if (geometryShaderSource)
    {
        makeFromFile(gShaderId_, GL_GEOMETRY_SHADER, geometryShaderSource);
        glAttachShader(programId_, gShaderId_);
    }

    // link program
    link();
    if (!linkStatus())
    {
        std::cerr << "[ERROR] Failed to link program." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Shader::createProgram()
{
    programId_ = glCreateProgram();

    if (!isCreated(programId_))
    {
        std::cerr << "[ERROR] Failed to instantiate program." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Shader::makeFromFile(GLuint &id, GLenum type, const char *fileName)
{
    std::string source{readFileFullText(fileName)};
    const char *c_str = source.c_str();

    // create
    id = glCreateShader(type);
    if (!isCreated(id))
    {
        std::cerr << "[ERROR] Failed to instantiate shader: "
                  << fileName
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    // compile
    glShaderSource(id, 1, &c_str, NULL);
    glCompileShader(id);

    if (!compileStatus(id))
    {
        std::cerr << "[ERROR] Failed to compile shader: "
                  <<  fileName
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    // attach
}

void Shader::link() { glLinkProgram(programId_); }

bool Shader::linkStatus()
{
    GLint status;
    glGetProgramiv(programId_, GL_LINK_STATUS, &status);

    return (status == GL_TRUE);
}

void Shader::use() noexcept { glUseProgram(programId_); }

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{ 
    glUniform3fv(glGetUniformLocation(programId_, name.c_str()), 1, &value[0]); 
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const
{ 
    glUniform3f(glGetUniformLocation(programId_, name.c_str()), x, y, z); 
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(programId_, name.c_str()),
                       1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setInt(const std::string &name, const int &value)
{
    glUniform1i(glGetUniformLocation(programId_, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value)
{ 
    glUniform1f(glGetUniformLocation(programId_, name.c_str()), value); 
}

} // namespace OpenGL

std::streampos getTextLength(std::ifstream &in)
{
    in.seekg(0, std::ios::end);
    std::streampos Length{in.tellg()};
    in.seekg(0, std::ios::beg);

    return Length;
}

bool readFileFullText(const char *fileName, std::string &outputText)
{
    std::ifstream in(fileName, std::ios::in | std::ios::binary);

    if (in.is_open() && in.good())
    {
        outputText.clear();
        outputText.resize(getTextLength(in));
        in.read(&(outputText[0]), outputText.size());
        in.close();

        return true;
    }

    return false;
}

std::string readFileFullText(const char *fileName)
{
    std::string output;

    if (readFileFullText(fileName, output))
    {
        return output;
    }

    return std::string();
}

inline bool compileStatus(GLuint id) noexcept
{
    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);

    return (status == GL_TRUE);
}

inline bool isCreated(GLuint id) noexcept { return static_cast<bool>(id); }

