#include "States/PlayState.hpp"
#include "States/GameOverState.hpp"
#include "Core/GameEngine.hpp"
#include "Systems/InputHandler.hpp"
#include "Systems/Network.hpp"
#include <cctype>

void PlayState::onEnter(GameEngine& engine) {
    GameState& data = engine.getGameData();
    data.gameStartTime = SDL_GetTicks();
    data.turnStartTime = SDL_GetTicks();
    engine.getAudio().stopMusic();

    // Initialize the Give Up button
    giveUpBtn = std::make_shared<Button>(SDL_Rect{WINDOW_WIDTH - 130, WINDOW_HEIGHT - 45, 120, 35}, "Give Up", engine.getContext().fontRegular);
}

void PlayState::handleInput(GameEngine& engine, const SDL_Event& event) {
    GameState& data = engine.getGameData();
    Audio& audio = engine.getAudio();

    // ISP/LSP applied to the give up button!
    if (giveUpBtn->handleInput(event)) {
        audio.playSound(engine.getContext().sfxWin);
        engine.changeState(std::make_unique<GameOverState>());
        return;
    }

    // ... [Rest of your original grid click and typing logic goes here] ...
}

void PlayState::render(GameEngine& engine) {
    // ... [Rest of your original grid rendering goes here] ...

    // Render the button using the LSP approach
    giveUpBtn->render(engine.getRenderer());
}