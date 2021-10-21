#include "glm/glm.hpp"
#include "Model/Mesh.hpp"

namespace Scene
{

enum Axis
{
    X = 0,
    Y = 1,
    Z = 2
};

class Component
{
public:
    explicit Component(Model::Mesh* model, glm::vec3 const &joint);

    void addChild(Component* child);
    void draw(glm::mat4 &view, glm::mat4 &projection);

    void translate(glm::vec3 const &dv);
    void rotate(float degrees, Scene::Axis axis);
    void reset();

private:
    Component* parent;
    std::vector<Component*> children;

    Model::Mesh* model;
    glm::vec3 joint;
    glm::vec3 ref[3]; // (ref-joint) is the director vector
    glm::mat4 transform;

    void propagate(glm::mat4 const &trans);
};

};
