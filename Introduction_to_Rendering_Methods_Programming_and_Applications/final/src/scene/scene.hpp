#ifndef SCENE_SCENE_HPP
#define SCENE_SCENE_HPP

#include "scene/environment.hpp"
#include "scene/object.hpp"
#include "scene/camera.hpp"
#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <memory>
#include <cmath>

namespace Scene
{

class Scene
{
public:
    struct Context
    {
        GLfloat G = (GLfloat)(6.67408 * std::pow(10,-11));
        GLfloat g = (GLfloat)(9.8);
    };

    explicit Scene(std::string sceneFile);
    std::vector<std::shared_ptr<Object>>& objects();
    const Context& context();
private:
    std::shared_ptr<Object> createObject(std::string info);
    std::shared_ptr<Object> createSphere(std::string info);
    std::shared_ptr<Object> createCube(std::string info);
    
    std::vector<std::shared_ptr<Object>> objects_;
    Context context_;
};

}

#endif // SCENE_SCENE_HPP
