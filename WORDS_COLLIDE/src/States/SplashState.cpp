#include "States/SplashState.hpp"
#include "States/MultiplayerMenuState.hpp"
#include "States/NameInputState.hpp"
#include "Core/GameEngine.hpp"
#include "Systems/InputHandler.hpp"

void SplashState::onEnter(GameEngine& engine) {
    engine.getAudio().playMusic(engine.getContext().bgmWelcome);
}

void SplashState::handleInput(GameEngine& engine, const SDL_Event& event) {
    GameState& data = engine.getGameData();

    data.startGameButton.isHovered = InputHandler::isMouseOverButton(event, data.startGameButton);
    data.multiplayerButton.isHovered = InputHandler::isMouseOverButton(event, data.multiplayerButton);

    if (InputHandler::isMouseClickOnButton(event, data.startGameButton)) {
        SDL_StartTextInput();
        engine.changeState(std::make_unique<NameInputState>());
    } else if (InputHandler::isMouseClickOnButton(event, data.multiplayerButton)) {
        engine.changeState(std::make_unique<MultiplayerMenuState>());
    }
}

void SplashState::update(GameEngine& engine, Uint32 deltaMs) {}

void SplashState::render(GameEngine& engine) {
    Renderer& renderer = engine.getRenderer();
    AppContext& ctx = engine.getContext();
    GameState& data = engine.getGameData();

    renderer.drawTextCentered(ctx.fontLarge, "WORDS_COLLIDE", 200, {100, 50, 150, 255});
    renderer.drawButton(data.startGameButton);
    renderer.drawButton(data.multiplayerButton);
}