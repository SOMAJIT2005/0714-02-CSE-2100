#include "States/IPInputState.hpp"
#include "States/MultiplayerMenuState.hpp"
#include "States/NameInputState.hpp"
#include "Core/GameEngine.hpp"
#include "Systems/InputHandler.hpp"
#include <cctype>

void IPInputState::handleInput(GameEngine& engine, const SDL_Event& event) {
    GameState& data = engine.getGameData();

    // Re-instantiate button locally or assume it's stored in the class like Phase 3/4.
    // For simplicity, here is the raw backspace/enter logic.
    if (InputHandler::isBackspace(event)) {
        if (!data.targetIP.empty()) data.targetIP.pop_back();
    } else if (InputHandler::isEnter(event)) {
        if (!data.targetIP.empty()) {
            // DIP: Using the injected Network interface!
            if (engine.getNetwork().joinGame(data.targetIP.c_str(), 8080)) {
                engine.changeState(std::make_unique<NameInputState>());
            } else {
                data.targetIP.clear();
            }
        }
    }

    char c;
    if (InputHandler::isAlphaNumericInput(event, c) && data.targetIP.size() < 15) {
        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
            data.targetIP += c;
        }
    }
}

void IPInputState::update(GameEngine& engine, Uint32 deltaMs) {}

void IPInputState::render(GameEngine& engine) {
    IRenderer& renderer = engine.getRenderer();
    AppContext& ctx = engine.getContext();
    GameState& data = engine.getGameData();

    renderer.drawTextCentered(ctx.fontLarge, "Join Game", 150, {100, 50, 150, 255});
    renderer.drawTextCentered(ctx.fontRegular, "Enter the Host's IP Address:", 250, {50, 50, 50, 255});

    SDL_Rect input_box_rect = {100, 300, WINDOW_WIDTH - 200, 50};
    renderer.drawRect(input_box_rect, {160, 130, 200, 255}, false);

    if (!data.targetIP.empty()) {
        renderer.drawText(ctx.fontRegular, data.targetIP, input_box_rect.x + 10, input_box_rect.y + 10, {0, 0, 0, 255});
    }

    renderer.drawTextCentered(ctx.fontRegular, "Press ENTER to Connect", 400, {50, 50, 50, 255});
}