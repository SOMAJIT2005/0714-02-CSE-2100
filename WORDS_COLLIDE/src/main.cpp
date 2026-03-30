#include "Core/GameEngine.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    GameEngine engine;

    if (!engine.init()) {
        std::cerr << "Fatal Error: Engine failed to initialize.\n";
        return 1;
    }

    engine.run();
    return 0;
}