#include "scene/scene.hpp"

#include "glm/glm.hpp"
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

#define GLM_ENABLE_EXPERIMENTAL

namespace Scene
{

Scene::Scene(std::string sceneFile)
{
    std::ifstream file(sceneFile);
    std::string line;

    int counter = 0;
    while (std::getline(file, line)) {
        if (line[0] == '#') { continue; }
        auto obj = createObject(line);
        obj->setId(counter++);
        objects_.push_back(obj);
    }
}

std::shared_ptr<Object> Scene::createObject(std::string info)
{
    std::stringstream infoIn(info);
    std::string type;
    infoIn >> type;

    if (type == "sphere")
    {
        return createSphere(info);
    }
    else if (type == "cube")
    {
        return createCube(info);
    }
    else
    {
        std::cerr << "[ERROR] Unknown object type: " << type << std::endl;
        exit(EXIT_FAILURE);
    }

    return nullptr;
}

std::shared_ptr<Object> Scene::createSphere(std::string info)
{
    std::stringstream infoIn(info);

    std::string type;
    GLfloat radius{0}, mass{0};
    glm::vec3 centroid{0};
    glm::vec3 velocity{0};
    bool movable{true};
    std::string texture_file;

    infoIn >> type 
           >> mass >> radius
           >> centroid.x >> centroid.y >> centroid.z
           >> velocity.x >> velocity.y >> velocity.z
           >> movable
           >> texture_file;

    Object::PhysicalState state{Object::Type::Sphere,
                                mass,
                                glm::vec3(radius),
                                {glm::vec3(0), glm::vec3(0), glm::vec3(0)}, 
                                centroid,
                                velocity,
                                movable};

    std::shared_ptr<Object> obj(new Object("resources/model/sphere.obj",
                                           texture_file.c_str(),
                                           state));
    
    std::cout << "type: " << (int)obj->state().type << std::endl;
    std::cout << "mass: " << obj->state().mass << std::endl;
    std::cout << "radius: " << glm::to_string(obj->state().radius) << std::endl;
    for (int i = 0; i <3; i++)
        std::cout << "normal: " << glm::to_string(obj->state().normals[i]) << std::endl;
    std::cout << "centroid: " << glm::to_string(obj->state().centroid) << std::endl;
    std::cout << "velocity: " << glm::to_string(obj->state().velocity) << std::endl;
    std::cout << "movable: " << obj->state().movable << std::endl;
    // std::cout << "model: " << glm::to_string(obj->model()) << std::endl;
    std::cout << "texture: " << texture_file << std::endl;

    return obj;
}

std::shared_ptr<Object> Scene::createCube(std::string info)
{
    std::stringstream infoIn(info);

    std::string type;
    GLfloat mass{0};
    glm::vec3 radius{0};
    glm::vec3 centroid{0};
    glm::vec3 velocity{0};
    bool movable{true};
    std::string texture_file;

    infoIn >> type 
           >> mass 
           >> radius.x >> radius.y >> radius.z
           >> centroid.x >> centroid.y >> centroid.z
           >> velocity.x >> velocity.y >> velocity.z
           >> movable
           >> texture_file;

    Object::PhysicalState state{Object::Type::Cube,
                                mass,
                                radius,
                                {glm::vec3(0), glm::vec3(0), glm::vec3(0)}, 
                                centroid,
                                velocity,
                                movable};

    std::shared_ptr<Object> obj(new Object("resources/model/cube.obj",
                                           texture_file.c_str(),
                                           state));
    
    std::cout << "type: " << (int)obj->state().type << std::endl;
    std::cout << "mass: " << obj->state().mass << std::endl;
    std::cout << "radius: " << glm::to_string(obj->state().radius) << std::endl;
    for (int i = 0; i <3; i++) 
        std::cout << "normal: " << glm::to_string(obj->state().normals[i]) << std::endl;
    std::cout << "centroid: " << glm::to_string(obj->state().centroid) << std::endl;
    std::cout << "velocity: " << glm::to_string(obj->state().velocity) << std::endl;
    std::cout << "movable: " << obj->state().movable << std::endl;
    // std::cout << "model: " << glm::to_string(obj->model()) << std::endl;
    std::cout << "texture: " << texture_file << std::endl;

    return obj;
}

std::vector<std::shared_ptr<Object>>& Scene::objects() { return objects_; }

const Scene::Context& Scene::context() { return context_; }

}
