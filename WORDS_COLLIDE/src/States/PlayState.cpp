#include "States/PlayState.hpp"
#include "States/GameOverState.hpp"
#include "Core/GameEngine.hpp"

void PlayState::onEnter(GameEngine& engine) {
    GameState& data = engine.getGameData();
    data.gameStartTime = SDL_GetTicks();
    data.turnStartTime = SDL_GetTicks();
    engine.getAudio().stopMusic();

    model = new PlayModel(data, engine.getScorer());
    view = new PlayView(model, engine.getRenderer(), engine.getAudio(), engine.getContext());
    view->initUI();
    controller = new PlayController(model, view, engine.getNetwork(), engine.getAudio(), engine.getContext());
}

void PlayState::handleInput(GameEngine& engine, const SDL_Event& event) {
    // If controller returns 1, the user clicked "Give Up"
    if (controller->processInput(event) == 1) {
        engine.changeState(std::make_unique<GameOverState>());
    }
}

void PlayState::update(GameEngine& engine, Uint32 deltaMs) {
    Uint32 currentTime = SDL_GetTicks();

    // Check if the overall game timer is done
    if (model->checkTimeUp(currentTime)) {
        engine.getAudio().playSound(engine.getContext().sfxWin);
        engine.changeState(std::make_unique<GameOverState>());
        return;
    }

    // Process rules and incoming network packets
    model->updateTimers(currentTime);
    controller->processNetwork();
    
    // Warning sound 
    GameState& data = model->getData();
    Uint32 elapsedMs = currentTime - data.gameStartTime;
    Uint32 remainingMs = GAME_DURATION_MS > elapsedMs ? GAME_DURATION_MS - elapsedMs : 0;
    
    if (remainingMs < WARNING_TIME_MS && currentTime - data.lastWarningTime > WARNING_INTERVAL_MS) {
        engine.getAudio().playSound(engine.getContext().sfxWarning);
        data.lastWarningTime = currentTime;
    }
}

void PlayState::render(GameEngine& engine) {
    view->draw();
}

PlayState::~PlayState() {
    delete controller;
    delete view;
    delete model;
}