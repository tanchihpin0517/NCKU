#include "engine/physics.hpp"

#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdio>
#include <algorithm>

GLfloat dist2PointPlane(const glm::vec3 &point, const glm::vec3 &plane, const glm::vec3 &normal);
GLfloat dist2PointPoint(const glm::vec3 &p1, const glm::vec3 &p2);
bool pointInFace(const glm::vec3 &point, const glm::vec3 &plane, const glm::vec3 &normal,
                 const std::array<glm::vec3, 3> &normals, const int n);

namespace Engine
{

PhysicsModule::PhysicsModule(unsigned int tick)
    : run_{true}, pause_{false},
      tick_{(GLfloat)(tick / 1000.0)}, updateInterval_{tick * 1000},
      threadNum_{1}
{}

void PhysicsModule::init()
{
    master_ = std::make_unique<MasterThread>(shared_from_this());
    for (unsigned int i=0; i<threadNum_; i++)
    {
        std::unique_ptr<WorkerThread> worker(new WorkerThread(shared_from_this()));
        workers_.push_back(std::move(worker));
    }
}

PhysicsModule::~PhysicsModule()
{
    finish();
}

void PhysicsModule::setScene(std::shared_ptr<Scene::Scene> scene)
{
    scene_ = scene;
    for (auto &object : scene_->objects())
    {
        // next state
        objectNextStates_.push_back(object->state());
        
        // collision
        std::vector<bool> collision;
        for (int i=0; i<(int)scene_->objects().size(); i++)
        {
            collision.push_back(false);
        }
        objectCollisionStates_.push_back(collision);

        // sink
        std::vector<GLfloat> sink;
        for (int i=0; i<(int)scene_->objects().size(); i++)
        {
            sink.push_back(0);
        }
        objectCollisionSink_.push_back(sink);

        // normal
        std::vector<glm::vec3> normal;
        for (int i=0; i<(int)scene_->objects().size(); i++)
        {
            normal.push_back(glm::vec3{0});
        }
        objectCollisionNormal_.push_back(normal);
    }
}

void PhysicsModule::start()
{
    master_->run(&PhysicsModule::simulate);
}

void PhysicsModule::pause()
{
}

void PhysicsModule::finish()
{
    run_ = false;
    if (!master_->join())
    {
        std::cerr << "Master thread isn't joinable." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void PhysicsModule::pushEvents()
{
    for (int i=0; i<(int)scene_->objects().size(); i++)
    {
        eventQueue_.push_back(i);
    }
}

void PhysicsModule::workersJoin()
{
    for (auto &worker : workers_) {
        if (!worker->join())
        {
            std::cerr << "Worker thread isn't joinable." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

int PhysicsModule::getEvent()
{
    int event = -1;
    eventMutex_.lock();
    if (eventQueue_.size() > 0)
    {
        event = eventQueue_.front();
        eventQueue_.pop_front();
    }
    eventMutex_.unlock();
    return event;
}

void PhysicsModule::simulate()
{
    std::chrono::duration<double, std::micro> elapsed;

    while (run_)
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        if (pause_) continue;

        simulationUpdate();

        while (true) // use spin lock for high persicision
        {
            auto t2 = std::chrono::high_resolution_clock::now();
            elapsed = t2 - t1;
            if (elapsed.count() >= updateInterval_) break;
        }
        // std::cout << elapsed.count() << std::endl;
        // printf("%f\n", elapsed.count());
    }
}

void PhysicsModule::simulationUpdate() // run by master
{
    if (!scene_)
    {
        std::cerr << "Scene is null while simulation is running" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (objectNextStates_.size() != scene_->objects().size())
    {
        std::cerr << "Size of current and next states mismatch" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!eventQueue_.empty())
    {
        std::cerr << "Event queue is not empty before updating" << std::endl;
        exit(EXIT_FAILURE);
    }

    // collision test
    pushEvents();
    for (auto &worker : workers_) { worker->run(&PhysicsModule::updateCollisionStates); }
    for (auto &worker : workers_) { worker->join(); }

    // for (int i=0; i<(int)scene_->objects().size(); i++)
    // {
    //     for (int j=0; j<(int)scene_->objects().size(); j++)
    //         std::cout << objectCollisionSink_[i][j] << " ";
    //     std::cout << std::endl;
    // }
    // std::cout << std::endl;

    // get next states
    pushEvents();
    for (auto &worker : workers_) { worker->run(&PhysicsModule::updateNextStates); }
    for (auto &worker : workers_) { worker->join(); }

    // update states
    for (int i=0; i<(int)scene_->objects().size(); i++)
    {
        auto object = scene_->objects()[i];
        object->displace(objectNextStates_[i].velocity * tick_);
        object->accelerate(objectNextStates_[i].velocity - object->state().velocity);
    }
    // std::cout << glm::to_string(scene_->objects()[0]->state().centroid) << std::endl;
}

void PhysicsModule::updateNextStates()
{
    while (true)
    {
        int event = getEvent();
        if (event == -1)
            break;

        auto object = scene_->objects()[event];

        // sum of forces
        glm::vec3 F{0};
        F += getGravity(object, scene_);
        F += getCollisionForce(object, scene_);

        // changes
        auto dv = getVelocityChange(object, F);

        // update states
        if (object->state().movable)
        {
            objectNextStates_[object->id()].velocity += dv;
        }

        // for (int i=0; i<(int)scene_->objects().size(); i++)
        // {
        //     if (objectCollisionStates_[object->id()][i])
        //     {
        //         objectNextStates_[object->id()].velocity = glm::vec3{0};
        //     }
        // }
    }
}

glm::vec3 PhysicsModule::getGravity(std::shared_ptr<Scene::Object> object,
                                    std::shared_ptr<Scene::Scene> scene)
{
    glm::vec3 F{0};
    for (auto other : scene->objects())
    {
        F += getGravity(object, other);
    }

    // context gravity: mg
    auto m = object->state().mass;
    auto g = scene->context().g;

    F += glm::vec3(0, 0, -m*g);

    return F;
}

glm::vec3 PhysicsModule::getGravity(std::shared_ptr<Scene::Object> s,
                                    std::shared_ptr<Scene::Object> t)
{
    glm::vec3 v = t->state().centroid - s->state().centroid; // directed
    GLfloat m1 = s->state().mass;
    GLfloat m2 = t->state().mass;
    GLfloat G = scene_->context().G;
    
    GLfloat r = glm::sqrt(glm::dot(v, v));
    glm::vec3 F = (G*m1*m2) / (r*r + eps) * (v/(r+eps));

    return F;
}

glm::vec3 PhysicsModule::getCollisionForce(std::shared_ptr<Scene::Object> object,
                                            std::shared_ptr<Scene::Scene> scene)
{
    glm::vec3 F{0};
    for (auto other : scene->objects())
    {
        F += getCollisionForce(object, other);
    }
    return F;
}

glm::vec3 PhysicsModule::getCollisionForce(std::shared_ptr<Scene::Object> obj1,
                                            std::shared_ptr<Scene::Object> obj2)
{
    if (objectCollisionStates_[obj1->id()][obj2->id()])
    {
        auto &sink = objectCollisionSink_[obj1->id()][obj2->id()];
        auto &normal = objectCollisionNormal_[obj1->id()][obj2->id()];

        glm::vec3 n = normal / glm::sqrt(glm::dot(normal, normal));
        // glm::vec3 v = obj1->state().velocity - obj2->state().velocity;

        sink = sink < 0 ? -sink : sink;
        // std::cout << glm::to_string(normal * 1000.0f) << std::endl;
        return sink * n * 1000.0f;
    }
    else
    {
        return glm::vec3{0};
    }
}

glm::vec3 PhysicsModule::getVelocityChange(std::shared_ptr<Scene::Object> object,
                                           glm::vec3 F)
{
    // F = ma -> a = F/m
    glm::vec3 a = F / (object->state().mass + eps);
    return a * tick_;
}

void PhysicsModule::updateCollisionStates()
{
    while (true)
    {
        int event = getEvent();
        if (event == -1)
            break;

        auto object = scene_->objects()[event];
        testCollision(object, scene_);
    }
}

void PhysicsModule::testCollision(std::shared_ptr<Scene::Object> object,
                                  std::shared_ptr<Scene::Scene> scene)
{
    for (auto other : scene->objects())
    {
        if (object->id() != other->id())
        {
            objectCollisionStates_[object->id()][other->id()] = testCollision(object, other);
        }
        else
        {
            objectCollisionStates_[object->id()][other->id()] = false;
        }
    }
}

bool PhysicsModule::testCollision(std::shared_ptr<Scene::Object> obj1,
                                  std::shared_ptr<Scene::Object> obj2)
{
    objectCollisionSink_[obj1->id()][obj2->id()] = 0;

    if (obj1->state().type == Scene::Object::Type::Sphere &&
        obj2->state().type == Scene::Object::Type::Sphere)
    {
        return collisionSphereSphere(obj1, obj2);
    }
    else if (obj1->state().type == Scene::Object::Type::Sphere &&
             obj2->state().type == Scene::Object::Type::Cube)
    {
        return collisionSphereCube(obj1, obj2);
    }
    else if (obj1->state().type == Scene::Object::Type::Cube &&
             obj2->state().type == Scene::Object::Type::Sphere)
    {
        return collisionSphereCube(obj2, obj1);
    }
    else if (obj1->state().type == Scene::Object::Type::Cube &&
             obj2->state().type == Scene::Object::Type::Cube)
    {

    }
    else
    {
        std::cerr << "[ERROR] Unknown collision type pair: " 
                  << (int)obj1->state().type << ", " << (int)obj2->state().type << std::endl;
        exit(EXIT_FAILURE);
    }
    return false;
}

bool PhysicsModule::collisionSphereSphere(std::shared_ptr<Scene::Object> sphere1,
                                          std::shared_ptr<Scene::Object> sphere2)
{
    GLfloat r1 = sphere1->state().radius.x;
    GLfloat r2 = sphere2->state().radius.x;

    glm::vec3 v = sphere1->state().centroid - sphere2->state().centroid;
    GLfloat dist2 = glm::dot(v, v);
    GLfloat sink = dist2 - (r1+r2)*(r1+r2);
    glm::vec3 normal = v;
    if (sink < 0)
    {
        setCollisionSink(sphere1, sphere2, sink);
        setCollisionNormal(sphere1, sphere2, normal);
        return true;
    }
    else
        return false;
}

bool PhysicsModule::collisionSphereCube(std::shared_ptr<Scene::Object> sphere,
                                        std::shared_ptr<Scene::Object> cube)
{
    bool collided = false;
    GLfloat r2 = glm::dot(sphere->state().normals[0], sphere->state().normals[0]);

    // face test
    for (int n=0; n<(int)cube->state().normals.size(); n++)
    {
        glm::vec3 normals[2];
        normals[0] = cube->state().normals[n];
        normals[1] = -cube->state().normals[n];

        for (auto &normal : normals)
        {
            glm::vec3 plane = cube->state().centroid + normal;
            GLfloat dist2 = dist2PointPlane(sphere->state().centroid, plane, normal);
            GLfloat sink = dist2 - r2;
            if (pointInFace(sphere->state().centroid, plane, normal, cube->state().normals, n) &&
                sink < 0)
            {
                setCollisionSink(sphere, cube, sink);
                setCollisionNormal(sphere, cube, normal);
                collided = true;
                // std::cout << "collided" << std::endl;
                // std::cout << glm::to_string(sphere->state().centroid - plane) << std::endl;
                // std::cout << glm::to_string(normal) << std::endl;
                // std::cout << glm::dot(sphere->state().centroid - plane, normal) << std::endl;
            }
        }
    }

    // corner test
    std::vector<glm::vec3> corners;
    std::array<GLfloat, 2> signs = {1, -1};
    for (auto s0 : signs)
        for (auto s1 : signs)
            for (auto s2 : signs)
                corners.push_back(cube->state().centroid + s0 * cube->state().normals[0]
                                                         + s1 * cube->state().normals[1]
                                                         + s2 * cube->state().normals[2]);
    for (const auto &corner : corners)
    {
        GLfloat sink = dist2PointPoint(corner, sphere->state().centroid) - r2;
        if (sink < 0)
        {
            glm::vec3 normal = sphere->state().centroid - corner;
            setCollisionSink(sphere, cube, sink);
            setCollisionNormal(sphere, cube, normal);
            collided = true;
        }
    }

    return collided;
}

// bool PhysicsModule::collisionCubeCube(std::shared_ptr<Scene::Object> obj1,
//                                       std::shared_ptr<Scene::Object> obj2)
// {
    
// }

void PhysicsModule::setCollisionSink(std::shared_ptr<Scene::Object> obj1,
                                    std::shared_ptr<Scene::Object> obj2,
                                    GLfloat sink)
{
    objectCollisionSink_[obj1->id()][obj2->id()] = 
        std::min(sink, objectCollisionSink_[obj1->id()][obj2->id()]);
    objectCollisionSink_[obj2->id()][obj1->id()] = 
        std::min(sink, objectCollisionSink_[obj2->id()][obj1->id()]);
}

void PhysicsModule::setCollisionNormal(std::shared_ptr<Scene::Object> obj1,
                                        std::shared_ptr<Scene::Object> obj2,
                                        glm::vec3 &normal)
{
    // std::cout << glm::to_string(normal * 1000.0f) << std::endl;

    objectCollisionNormal_[obj1->id()][obj2->id()] = normal;
    objectCollisionNormal_[obj2->id()][obj1->id()] = -normal;
}

PhysicsModule::MasterThread::MasterThread(std::shared_ptr<PhysicsModule> physics)
    : physics_{physics}, thread_{nullptr}
{}

template<class Function>
void PhysicsModule::MasterThread::run(Function&& f)
{
    if (join())
    {
        std::cerr << "Master thread runs before joined" << std::endl;
        exit(EXIT_FAILURE);
    }
    thread_.reset(new std::thread(f, physics_.lock().get()));
}

bool PhysicsModule::MasterThread::join()
{
    if (thread_ && thread_->joinable())
    {
        thread_->join();
        return true;
    }
    return false;
}

static int idCount = 0;
PhysicsModule::WorkerThread::WorkerThread(std::shared_ptr<PhysicsModule> physics)
    : physics_{physics}, thread_{nullptr}
{
    id_ = idCount++;
}

template<class Function>
void PhysicsModule::WorkerThread::run(Function&& f)
{
    if (join())
    {
        std::cerr << "Worker thread runs before joined" << std::endl;
        exit(EXIT_FAILURE);
    }
    thread_.reset(new std::thread(f, physics_.lock().get()));
}

bool PhysicsModule::WorkerThread::join()
{
    if (thread_ && thread_->joinable())
    {
        thread_->join();
        return true;
    }
    return false;
}

} // namespace Engine

bool pointInFace(const glm::vec3 &point, const glm::vec3 &plane, const glm::vec3 &normal,
                 const std::array<glm::vec3, 3> &normals, const int n)
{
    // if not on normal positive direction
    if (glm::dot(normal, point-plane) < 0)
    {
        return false;
    }

    std::vector<glm::vec3> notPlaneNormals;
    for (int i=0; i<3; i++) if (i != n) notPlaneNormals.push_back(normals[i]);
    glm::vec3 corners[4];
    corners[0] = plane + notPlaneNormals[0] + notPlaneNormals[1];
    corners[1] = plane - notPlaneNormals[0] + notPlaneNormals[1];
    corners[2] = plane - notPlaneNormals[0] - notPlaneNormals[1];
    corners[3] = plane + notPlaneNormals[0] - notPlaneNormals[1];
    for (int i=0; i<4; i++)
    {
        glm::vec3 e[2];
        e[0] = corners[(i+4+1) % 4] - corners[i];
        e[1] = corners[(i+4-1) % 4] - corners[i];
        for (int j=0; j<2; j++)
        {
            glm::vec3 v = point - corners[i];
            if (glm::dot(e[j], v) < 0)
                return false;
        }
    }
    return true;

}

GLfloat dist2PointPlane(const glm::vec3 &point, const glm::vec3 &plane, const glm::vec3 &normal)
{
    // notice that normal is NOT normalized :)

    // std::cout << "dist2PointPlane" << std::endl;
    // std::cout << glm::to_string(point) << std::endl;
    // std::cout << glm::to_string(plane) << std::endl;
    // std::cout << glm::to_string(normal) << std::endl;


    glm::vec3 v = point - plane;
    GLfloat d2 = glm::dot(v, normal) * glm::dot(v, normal) / (glm::dot(v, v), glm::dot(normal, normal));
    return d2;
}

GLfloat dist2PointPoint(const glm::vec3 &p1, const glm::vec3 &p2)
{
    return glm::dot(p1-p2, p1-p2);
}