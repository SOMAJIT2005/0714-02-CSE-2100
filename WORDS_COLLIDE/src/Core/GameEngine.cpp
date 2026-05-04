#include "Core/GameEngine.hpp"
#include "States/SplashState.hpp"

GameEngine::GameEngine(AppContext context, 
                       std::unique_ptr<IAudio> audioSystem,
                       std::unique_ptr<IRenderer> renderSystem,
                       std::unique_ptr<INetwork> networkSystem,
                       std::unique_ptr<IBoardScorer> scorerSystem)
    : app(context),
      audio(std::move(audioSystem)), 
      renderer(std::move(renderSystem)),
      network(std::move(networkSystem)),
      scorer(std::move(scorerSystem)),
      isRunning(false), lastTime(0) {
}

GameEngine::~GameEngine() {
    network->cleanup();
}

bool GameEngine::init() {
    network->init();
    audio->init();

    state.currentState = GameStateType::SPLASH;
    state.gameStartTime = 0;
    state.scores = {0, 0};
    for (auto &row : state.grid) row.fill(0);

    isRunning = true;
    lastTime = SDL_GetTicks();
    changeState(std::make_unique<SplashState>());

    return true;
}

void GameEngine::changeState(std::unique_ptr<IGameState> newState) {
    if (currentState) currentState->onExit(*this);
    currentState = std::move(newState);
    if (currentState) currentState->onEnter(*this);
}

void GameEngine::run() {
    SDL_Event event;
    while (isRunning) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 deltaMs = currentTime - lastTime;
        lastTime = currentTime;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit();
            else if (currentState) currentState->handleInput(*this, event);
        }

        if (currentState) currentState->update(*this, deltaMs);

        if (currentState) {
            renderer->clear({230, 210, 255, 255});
            currentState->render(*this);
            renderer->present();
        }
        SDL_Delay(10);
    }
}
void GameEngine::quit() { isRunning = false; }