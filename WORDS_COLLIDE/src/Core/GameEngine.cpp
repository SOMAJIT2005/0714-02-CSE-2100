#include "Core/GameEngine.hpp"

// We reordered the initialization list here to exactly match the header, fixing the -Wreorder warning!
GameEngine::GameEngine(AppContext ctx,
                       std::unique_ptr<Audio> audioSystem,
                       std::unique_ptr<Renderer> renderSystem,
                       std::unique_ptr<Network> networkSystem,
                       std::unique_ptr<IBoardScorer> boardScorer)
    : context(ctx),
      audio(std::move(audioSystem)),
      renderer(std::move(renderSystem)),
      network(std::move(networkSystem)),
      scorer(std::move(boardScorer)),
      isRunning(true) 
{
}

GameEngine::~GameEngine() {
    // Unique_ptrs automatically clean up memory (RAII principle), 
    // but if you have specific cleanup, do it here.
}

void GameEngine::changeState(std::unique_ptr<IGameState> newState) {
    nextState = std::move(newState);
}

void GameEngine::quit() {
    isRunning = false;
}

void GameEngine::run() {
    Uint32 lastTime = SDL_GetTicks();

    while (isRunning) {
        // 1. Handle State Transitions
        if (nextState) {
            if (currentState) {
                currentState->onExit(*this);
            }
            currentState = std::move(nextState);
            currentState->onEnter(*this);
        }

        // 2. Calculate Delta Time
        Uint32 currentTime = SDL_GetTicks();
        Uint32 deltaMs = currentTime - lastTime;
        lastTime = currentTime;

        // 3. Handle Events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit();
            }
            if (currentState) {
                currentState->handleInput(*this, event);
            }
        }

        // 4. Update Logic
        if (currentState) {
            currentState->update(*this, deltaMs);
        }

        // 5. Render
        renderer->clear({230, 210, 255, 255}); // Light purple background
        if (currentState) {
            currentState->render(*this);
        }
        renderer->present();

        // Optional: Small delay to cap framerate and save CPU
        SDL_Delay(16);
    }
}