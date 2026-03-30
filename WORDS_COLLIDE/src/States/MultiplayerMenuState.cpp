#include "States/MultiplayerMenuState.hpp"
#include "States/SplashState.hpp"
#include "States/IPInputState.hpp"
#include "States/NameInputState.hpp"
#include "Core/GameEngine.hpp"
#include "Systems/Network.hpp"

void MultiplayerMenuState::onEnter(GameEngine& engine) {
    TTF_Font* fontReg = engine.getContext().fontRegular;
    TTF_Font* fontLrg = engine.getContext().fontLarge;

    drawables.push_back(std::make_shared<Label>("Multiplayer Setup", (WINDOW_WIDTH - 380) / 2, 150, fontLrg, SDL_Color{100, 50, 150, 255}));
    drawables.push_back(std::make_shared<Label>("Make sure both devices are on the same Wi-Fi!", (WINDOW_WIDTH - 450) / 2, 250, fontReg, SDL_Color{50, 50, 50, 255}));

    hostBtn = std::make_shared<Button>(SDL_Rect{(WINDOW_WIDTH - 250) / 2, 330, 250, 50}, "Host a Game", fontReg);
    joinBtn = std::make_shared<Button>(SDL_Rect{(WINDOW_WIDTH - 250) / 2, 410, 250, 50}, "Join a Game", fontReg);
    backBtn = std::make_shared<Button>(SDL_Rect{(WINDOW_WIDTH - 250) / 2, 490, 250, 50}, "Back", fontReg);

    drawables.push_back(hostBtn);
    drawables.push_back(joinBtn);
    drawables.push_back(backBtn);
}

void MultiplayerMenuState::handleInput(GameEngine& engine, const SDL_Event& event) {
    if (hostBtn->handleInput(event)) {
        if (Network::hostGame(8080)) {
            SDL_StartTextInput();
            engine.changeState(std::make_unique<NameInputState>());
        }
    } else if (joinBtn->handleInput(event)) {
        engine.getGameData().targetIP.clear();
        SDL_StartTextInput();
        engine.changeState(std::make_unique<IPInputState>());
    } else if (backBtn->handleInput(event)) {
        engine.changeState(std::make_unique<SplashState>());
    }
}

void MultiplayerMenuState::update(GameEngine& engine, Uint32 deltaMs) {}

void MultiplayerMenuState::render(GameEngine& engine) {
    for (const auto& item : drawables) item->render(engine.getRenderer());
}