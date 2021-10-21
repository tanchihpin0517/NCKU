#ifndef SCENE_OBJECT_HPP
#define SCENE_OBJECT_HPP

#include "opengl/shader.hpp"
#include "opengl/texture.hpp"
#include "opengl/mesh.hpp"
#include "glm/glm.hpp"

#include <string>
#include <memory>
#include <array>

const glm::mat4 mat4I{1};
const glm::vec3 vec3Z{0};
const glm::vec3 vec3I{1};

namespace Scene
{

class Object
{
public:
    enum Type
    {
        None,
        Sphere,
        Cube
    };

    struct PhysicalState
    {
        Type type = Type::None;
        GLfloat mass = 0;
        glm::vec3 radius = vec3I; // for cube, radius is (side length / 2)
        std::array<glm::vec3, 3> normals = {glm::vec3{1,0,0}, glm::vec3{0,1,0}, glm::vec3{0,0,1}};
        glm::vec3 centroid = vec3Z;
        glm::vec3 velocity = vec3Z;
        bool movable = true;
    };

    explicit Object(const char *modelSource,
                    const char *textureSource,
                    PhysicalState state);
    void bind();
    void release();
    int id();
    void setId(int id);
    
    GLsizei indicesCount();
    glm::mat4 model() const;
    const PhysicalState& state();

    void displace(glm::vec3 diff);
    void displace(float dx, float dy, float dz);
    void accelerate(glm::vec3 diff);
    void accelerate(float dx, float dy, float dz);
    void scale(glm::vec3 diff);
    void scale(float dx, float dy, float dz);
private:
    int id_;

    std::unique_ptr<OpenGL::Texture> texture_;
    std::unique_ptr<OpenGL::Mesh> mesh_;
    GLsizei indicesCount_;
    glm::mat4 mModel_;

    PhysicalState state_;
};

}

#endif // SCENE_OBJECT_HPP
