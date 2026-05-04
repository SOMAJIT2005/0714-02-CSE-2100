#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include "Core/Types.hpp"
#include "Core/IGameState.hpp"
#include "Systems/Audio.hpp"
#include "Systems/Renderer.hpp"
#include "Systems/Network.hpp"
#include "Game/IBoardScorer.hpp"
#include <memory>

class GameEngine {
public:
    // DEPENDENCY INJECTION!
    GameEngine(AppContext context, 
               std::unique_ptr<Audio> audioSystem,
               std::unique_ptr<Renderer> renderSystem,
               std::unique_ptr<Network> networkSystem,
               std::unique_ptr<IBoardScorer> scorerSystem);
    ~GameEngine();

    bool init();
    void run();
    void quit();
    void changeState(std::unique_ptr<IGameState> newState);

    GameState& getGameData() { return state; }
    AppContext& getContext() { return app; }
    
    // Returning interfaces!
    Audio& getAudio() { return *audio; }
    Renderer& getRenderer() { return *renderer; }
    Network& getNetwork() { return *network; }
    IBoardScorer& getScorer() { return *scorer; }

private:
    bool isRunning;
    Uint32 lastTime;
    
    std::unique_ptr<IGameState> currentState;

    AppContext app;
    GameState state;

    // Holding abstractions, not concrete details
    std::unique_ptr<Audio> audio;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Network> network;
    std::unique_ptr<IBoardScorer> scorer;
};

#endif // GAME_ENGINE_HPP