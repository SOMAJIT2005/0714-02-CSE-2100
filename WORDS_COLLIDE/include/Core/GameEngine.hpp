#pragma once
#include "Core/Config.hpp"
#include "Core/IGameState.hpp"
#include "Systems/Audio.hpp"
#include "Systems/Renderer.hpp"
#include "Systems/Network.hpp"
#include "Game/IBoardScorer.hpp" // Ensure this matches your file name!
#include <memory>
#include <SDL2/SDL.h>

class GameEngine {
private:
    AppContext context;
    GameState gameData;
    
    std::unique_ptr<Audio> audio;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Network> network;
    std::unique_ptr<IBoardScorer> scorer;

    std::unique_ptr<IGameState> currentState;
    std::unique_ptr<IGameState> nextState;

    bool isRunning;

public:
    // Constructor uses Dependency Injection (SOLID)
    GameEngine(AppContext ctx, 
               std::unique_ptr<Audio> audioSystem,
               std::unique_ptr<Renderer> renderSystem,
               std::unique_ptr<Network> networkSystem,
               std::unique_ptr<IBoardScorer> boardScorer);
    
    ~GameEngine();

    void run();
    void quit();
    void changeState(std::unique_ptr<IGameState> newState);

    // Getters for subsystems
    AppContext& getContext() { return context; }
    GameState& getGameData() { return gameData; }
    Audio& getAudio() { return *audio; }
    Renderer& getRenderer() { return *renderer; }
    Network& getNetwork() { return *network; }
    IBoardScorer& getScorer() { return *scorer; }
};