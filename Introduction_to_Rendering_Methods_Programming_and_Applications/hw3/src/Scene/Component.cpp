#include "Scene/Component.hpp"
#include "glm/gtx/io.hpp"
#include <iostream>

Scene::Component::Component(Model::Mesh* model, glm::vec3 const &joint)
    : model{model}, joint{joint}, transform{1}, parent{NULL}
{
    ref[Scene::Axis::X] = glm::vec3(joint); ref[Scene::Axis::X][0] += 1;
    ref[Scene::Axis::Y] = glm::vec3(joint); ref[Scene::Axis::Y][1] += 1;
    ref[Scene::Axis::Z] = glm::vec3(joint); ref[Scene::Axis::Z][2] += 1;
}

void Scene::Component::addChild(Component* child)
{
    children.push_back(child);
    child->parent = this;
}

void Scene::Component::draw(glm::mat4 &view, glm::mat4 &projection)
{
    glm::mat4 prev = model->model();
    model->setModel(transform * prev);
    model->draw(view, projection);
    model->setModel(prev);

    for (auto &child : children)
    {
        child->draw(view, projection);
    }
}

void Scene::Component::translate(glm::vec3 const &dv)
{
    glm::mat4 M = glm::translate(glm::mat4(1), dv);
    propagate(M);
}

void Scene::Component::rotate(float degrees, Scene::Axis axis)
{
    float radians = glm::radians(degrees);
    glm::vec3 ref_vec = ref[axis] - joint;

    glm::mat4 T = glm::translate(glm::mat4(1), -joint);
    glm::mat4 R = glm::rotate(glm::mat4(1), radians, ref_vec);
    glm::mat4 M = glm::inverse(T) * R * T;
    propagate(M);
}

void Scene::Component::reset()
{
    glm::mat4 M = glm::inverse(transform);
    propagate(M);
    for (auto const& child : children)
    {
        child->reset();
    }
}

void Scene::Component::propagate(glm::mat4 const &M)
{
    transform = M * transform;
    joint = glm::vec3((M * glm::vec4(joint, 1.0)));
    ref[0] = glm::vec3((M * glm::vec4(ref[0], 1.0)));
    ref[1] = glm::vec3((M * glm::vec4(ref[1], 1.0)));
    ref[2] = glm::vec3((M * glm::vec4(ref[2], 1.0)));
    //model->setModel(transform*model->model_);
    for (auto const& child : children)
    {
        child->propagate(M);
    }
}
