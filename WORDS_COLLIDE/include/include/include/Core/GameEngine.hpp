#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include "Core/Types.hpp"
#include "Core/IGameState.hpp"
#include "Systems/IAudio.hpp"
#include "Systems/IRenderer.hpp"
#include "Systems/INetwork.hpp"
#include "Game/IBoardScorer.hpp"
#include <memory>

class GameEngine {
public:
    // DEPENDENCY INJECTION!
    GameEngine(AppContext context, 
               std::unique_ptr<IAudio> audioSystem,
               std::unique_ptr<IRenderer> renderSystem,
               std::unique_ptr<INetwork> networkSystem,
               std::unique_ptr<IBoardScorer> scorerSystem);
    ~GameEngine();

    bool init();
    void run();
    void quit();
    void changeState(std::unique_ptr<IGameState> newState);

    GameState& getGameData() { return state; }
    AppContext& getContext() { return app; }
    
    // Returning interfaces!
    IAudio& getAudio() { return *audio; }
    IRenderer& getRenderer() { return *renderer; }
    INetwork& getNetwork() { return *network; }
    IBoardScorer& getScorer() { return *scorer; }

private:
    bool isRunning;
    Uint32 lastTime;
    
    std::unique_ptr<IGameState> currentState;

    AppContext app;
    GameState state;

    // Holding abstractions, not concrete details
    std::unique_ptr<IAudio> audio;
    std::unique_ptr<IRenderer> renderer;
    std::unique_ptr<INetwork> network;
    std::unique_ptr<IBoardScorer> scorer;
};

#endif // GAME_ENGINE_HPP