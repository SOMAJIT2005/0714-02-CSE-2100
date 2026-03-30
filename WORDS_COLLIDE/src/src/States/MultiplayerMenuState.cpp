#include "States/MultiplayerMenuState.hpp"
#include "States/SplashState.hpp"
#include "States/IPInputState.hpp"
#include "States/NameInputState.hpp"
#include "Core/GameEngine.hpp"
#include "Systems/InputHandler.hpp"
#include "Systems/Network.hpp"

void MultiplayerMenuState::handleInput(GameEngine& engine, const SDL_Event& event) {
    GameState& data = engine.getGameData();

    data.hostGameButton.isHovered = InputHandler::isMouseOverButton(event, data.hostGameButton);
    data.joinGameButton.isHovered = InputHandler::isMouseOverButton(event, data.joinGameButton);
    data.backButton.isHovered = InputHandler::isMouseOverButton(event, data.backButton);

    if (InputHandler::isMouseClickOnButton(event, data.hostGameButton)) {
        if (Network::hostGame(8080)) {
            SDL_StartTextInput();
            engine.changeState(std::make_unique<NameInputState>());
        }
    } else if (InputHandler::isMouseClickOnButton(event, data.joinGameButton)) {
        data.targetIP.clear();
        SDL_StartTextInput();
        engine.changeState(std::make_unique<IPInputState>());
    } else if (InputHandler::isMouseClickOnButton(event, data.backButton)) {
        engine.changeState(std::make_unique<SplashState>());
    }
}

void MultiplayerMenuState::update(GameEngine& engine, Uint32 deltaMs) {}

void MultiplayerMenuState::render(GameEngine& engine) {
    Renderer& renderer = engine.getRenderer();
    AppContext& ctx = engine.getContext();
    GameState& data = engine.getGameData();

    renderer.drawTextCentered(ctx.fontLarge, "Multiplayer Setup", 150, {100, 50, 150, 255});
    renderer.drawTextCentered(ctx.fontRegular, "Make sure both devices are on the same Wi-Fi!", 250, {50, 50, 50, 255});
    
    renderer.drawButton(data.hostGameButton);
    renderer.drawButton(data.joinGameButton);
    renderer.drawButton(data.backButton);
}