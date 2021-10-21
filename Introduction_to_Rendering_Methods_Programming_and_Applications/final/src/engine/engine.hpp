#ifndef ENGINE_ENGINE_HPP
#define ENGINE_ENGINE_HPP

#include "engine/render.hpp"
#include "engine/illumination.hpp"
#include "engine/physics.hpp"
#include "scene/scene.hpp"

#include <string>
#include <memory>

namespace Engine
{

class Engine
{
public:
    explicit Engine();
    ~Engine();

    Engine(const Engine &other) = delete;
    Engine &operator=(const Engine &other) = delete;

    void loadScene(std::string sceneFile);
    void start();
    void finish();
private:
    std::shared_ptr<Scene::Scene> scene_;
    std::unique_ptr<RenderModule> renderModule_;
    std::shared_ptr<PhysicsModule> physicsModule_;
};

}

#endif // ENGINE_ENGINE_HPP
