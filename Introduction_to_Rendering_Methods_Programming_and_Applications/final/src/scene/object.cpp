#include "scene/object.hpp"

#include "opengl/texture.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "tiny_obj_loader.h"

#include <iostream>
#include <sstream>

namespace Scene
{

Object::Object(const char *modelSource,
               const char *textureSource,
               PhysicalState state)
    : id_{-1}, texture_{nullptr}, mesh_{nullptr},
      indicesCount_{0}, mModel_{mat4I}, state_{}
{
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string errorMessage;

    auto success =
        tinyobj::LoadObj(shapes, materials, errorMessage, modelSource);

    if (!success)
    {
        std::cerr << "[Error]" << errorMessage.c_str();
        exit(EXIT_FAILURE);
    }

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> textureCoordinates;
    std::vector<unsigned int> indices;

    for (auto &shape : shapes)
    {
        positions.insert(positions.end(), shape.mesh.positions.begin(),
                         shape.mesh.positions.end());
        if (!shape.mesh.normals.empty())
        {
            normals.insert(normals.end(), shape.mesh.normals.begin(),
                           shape.mesh.normals.end());
        }

        if (!shape.mesh.texcoords.empty())
        {
            textureCoordinates.insert(textureCoordinates.end(),
                                      shape.mesh.texcoords.begin(),
                                      shape.mesh.texcoords.end());
        }

        indices.insert(indices.end(), shape.mesh.indices.begin(),
                       shape.mesh.indices.end());
    }

    indicesCount_ = static_cast<GLsizei>(indices.size());
    texture_.reset(new OpenGL::Texture(textureSource));
    mesh_.reset(new OpenGL::Mesh(positions, normals, textureCoordinates, indices));

    // initialize state
    state_.type = state.type;
    state_.mass = state.mass;
    scale(state.radius);
    displace(state.centroid);
    accelerate(state.velocity);
    state_.movable = state.movable;
}

void Object::bind()
{
    mesh_->bind();
    texture_->bind();
}

void Object::release()
{
    mesh_->release();
    texture_->release();
}

GLsizei Object::indicesCount()
{
    return indicesCount_;
}

glm::mat4 Object::model() const
{
    return mModel_;
}

void Object::displace(glm::vec3 diff)
{
    state_.centroid += diff;

    glm::mat4 T = glm::translate(mat4I, diff);
    mModel_ = T * mModel_;
}

void Object::displace(float dx, float dy, float dz)
{
    auto diff = glm::vec3(dx, dy, dz);
    displace(diff);
}

void Object::accelerate(glm::vec3 diff)
{
    state_.velocity += diff;
}

void Object::accelerate(float dx, float dy, float dz)
{
    auto diff = glm::vec3(dx, dy, dz);
    accelerate(diff);
}

void Object::scale(glm::vec3 diff)
{
    state_.radius *= diff;
    for (int i=0; i<3; i++)
    {
        state_.normals[i] *= diff;
    }
    // state_.radius.x *= diff.x;
    // state_.radius.y *= diff.y;
    // state_.radius.z *= diff.z;

    glm::mat4 T = glm::scale(mat4I, diff);
    mModel_ = T * mModel_;
}

void Object::scale(float dx, float dy, float dz)
{
    auto diff = glm::vec3(dx, dy, dz);
    scale(diff);
}

const Object::PhysicalState& Object::state()
{
    return state_;
}

int Object::id() { return id_; }

void Object::setId(int id) { id_ = id; }

} // namespace Scene
