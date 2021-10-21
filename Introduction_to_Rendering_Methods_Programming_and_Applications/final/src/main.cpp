#include "engine/engine.hpp"
#include "engine/physics.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

// int main(int argc, char *argv[])
int main()
{
    // if (argc <= 1)
    // {
    //     std::cerr << "Not enough parameter\n";
    //     std::cerr << "Expect: " << argv[0]
    //               << "[scene file]"
    //               << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    // std::string sceneFile{argv[1]};

    // std::cout << "Scene File: " << sceneFile << "\n"
    //           << std::endl;

    Engine::Engine engine;
    engine.loadScene("resources/scene_3.txt");
    engine.start();
    
    return 0;
}
