#include "engine/engine.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <iostream>

namespace Engine
{

Engine::Engine()
{
    std::array<int, 2> openglVersion{3,3};
    std::array<int, 2> windowSize{1024, 768};
    std::string windowTitle{"Simulation"};
    unsigned int tick{1};

    renderModule_.reset(new RenderModule(openglVersion,
                                         windowSize,
                                         windowTitle));

    physicsModule_ = std::make_shared<PhysicsModule>(tick);
    physicsModule_->init();
    
}

Engine::~Engine() { finish(); }

void Engine::loadScene(std::string sceneFile)
{
    scene_ = std::make_shared<Scene::Scene>(sceneFile);
    physicsModule_->setScene(scene_);
}

void Engine::start()
{
    physicsModule_->start();
    renderModule_->loop(scene_);
}

void Engine::finish() {}

}
