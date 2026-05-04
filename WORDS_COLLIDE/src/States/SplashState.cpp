#include "States/SplashState.hpp"
#include "States/MultiplayerMenuState.hpp"
#include "States/NameInputState.hpp"
#include "Core/GameEngine.hpp"
#include "Core/Config.hpp"

void SplashState::onEnter(GameEngine& engine) {
    engine.getAudio().playMusic(engine.getContext().bgmWelcome);
    TTF_Font* fontReg = engine.getContext().fontRegular;
    TTF_Font* fontLrg = engine.getContext().fontLarge;

    auto titleLabel = std::make_shared<Label>("WORDS_COLLIDE", (WINDOW_WIDTH - 380) / 2, 200, fontLrg, SDL_Color{100, 50, 150, 255});
    
    startBtn = std::make_shared<Button>(SDL_Rect{(WINDOW_WIDTH - 250) / 2, 350, 250, 50}, "Play on One Device", fontReg);
    multiBtn = std::make_shared<Button>(SDL_Rect{(WINDOW_WIDTH - 250) / 2, 430, 250, 50}, "Multiplayer (Wi-Fi)", fontReg);

    drawables.push_back(titleLabel);
    drawables.push_back(startBtn);
    drawables.push_back(multiBtn);
}

void SplashState::handleInput(GameEngine& engine, const SDL_Event& event) {
    if (startBtn->handleInput(event)) {
        SDL_StartTextInput();
        engine.changeState(std::make_unique<NameInputState>());
    } 
    else if (multiBtn->handleInput(event)) {
        engine.changeState(std::make_unique<MultiplayerMenuState>());
    }
}

void SplashState::update(GameEngine& engine, Uint32 deltaMs) {}

void SplashState::render(GameEngine& engine) {
    Renderer& renderer = engine.getRenderer();
    for (const auto& item : drawables) {
        item->render(renderer);
    }
}