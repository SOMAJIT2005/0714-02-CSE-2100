#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include "Core/Types.hpp"
#include "Game/Dictionary.hpp"
#include "Game/BoardScorer.hpp"
#include "Systems/Audio.hpp"
// Note: Graphics/Network includes will go here as we finish migrating them

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    bool init();
    void run();

private:
    void processNetwork();
    void updateTimers(Uint32 currentTime);

    bool isRunning;
    AppContext app;
    GameState state;
    Dictionary dictionary;
    BoardScorer scorer;
    Audio audio;
    // Renderer renderer;
};

#endif // GAME_ENGINE_HPP