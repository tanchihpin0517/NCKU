#ifndef ENGINE_PHYSICS_HPP
#define ENGINE_PHYSICS_HPP

#include "scene/scene.hpp"

#include <memory>
#include <thread>
#include <vector>
#include <deque>
#include <mutex>
#include <cmath>

namespace Engine
{

class PhysicsModule: public std::enable_shared_from_this<PhysicsModule>
{
public:
    const GLfloat eps = (GLfloat)std::pow(10,-9); // avoid divided by zero
    struct Context
    {
       
    };

    class MasterThread
    {
    public:
        MasterThread(std::shared_ptr<PhysicsModule> physics);
        template< class Function> void run(Function&& f);
        bool join();
    private:
        std::weak_ptr<PhysicsModule> physics_;
        std::unique_ptr<std::thread> thread_;
    };

    class WorkerThread
    {
    public:
        WorkerThread(std::shared_ptr<PhysicsModule> physics);
        template< class Function> void run(Function&& f);
        bool join();
    private:
        int id_;
        std::weak_ptr<PhysicsModule> physics_;
        std::unique_ptr<std::thread> thread_;
    };

    PhysicsModule(unsigned int tick); // tick: ms
    void init();
    ~PhysicsModule();
    void setScene(std::shared_ptr<Scene::Scene> scene);
    void start();
    void pause();
    void finish();
    void simulate();
private:
    void pushEvents();
    void workersJoin();
    int getEvent();

    void simulationUpdate();
    void updateCollisionStates();
    void updateNextStates();

    glm::vec3 getGravity(std::shared_ptr<Scene::Object> object,
                         std::shared_ptr<Scene::Scene> scene);
    glm::vec3 getGravity(std::shared_ptr<Scene::Object> obj1,
                         std::shared_ptr<Scene::Object> obj2);
    glm::vec3 getCollisionForce(std::shared_ptr<Scene::Object> object,
                                std::shared_ptr<Scene::Scene> scene);
    glm::vec3 getCollisionForce(std::shared_ptr<Scene::Object> obj1,
                                std::shared_ptr<Scene::Object> obj2);
    glm::vec3 getVelocityChange(std::shared_ptr<Scene::Object> object,
                                glm::vec3 F);
    void testCollision(std::shared_ptr<Scene::Object> obj1,
                       std::shared_ptr<Scene::Scene> scene);
    bool testCollision(std::shared_ptr<Scene::Object> obj1,
                       std::shared_ptr<Scene::Object> obj2);
    bool collisionSphereSphere(std::shared_ptr<Scene::Object> obj1,
                                 std::shared_ptr<Scene::Object> obj2);
    bool collisionSphereCube(std::shared_ptr<Scene::Object> obj1,
                               std::shared_ptr<Scene::Object> obj2);
    bool collisionCubeCube(std::shared_ptr<Scene::Object> obj1,
                             std::shared_ptr<Scene::Object> obj2);

    void setCollisionSink(std::shared_ptr<Scene::Object> obj1,
                            std::shared_ptr<Scene::Object> obj2,
                            GLfloat sink);
    void setCollisionNormal(std::shared_ptr<Scene::Object> obj1,
                            std::shared_ptr<Scene::Object> obj2,
                            glm::vec3 &normal);

    std::unique_ptr<MasterThread> master_;
    std::vector<std::unique_ptr<WorkerThread>> workers_;
    std::mutex eventMutex_;
    std::mutex updateMutex_;

    std::shared_ptr<Scene::Scene> scene_;
    std::vector<Scene::Object::PhysicalState> objectNextStates_;
    std::vector<std::vector<bool> > objectCollisionStates_;
    std::vector<std::vector<GLfloat> > objectCollisionSink_;
    std::vector<std::vector<glm::vec3> > objectCollisionNormal_;
    std::deque<int> eventQueue_;
    
    bool run_;
    bool pause_;

    GLfloat tick_; // (s) time passed between two simulation states
    unsigned int updateInterval_;
    unsigned int threadNum_; // number of threads
};

}

#endif // ENGINE_PHYSICS_HPP
