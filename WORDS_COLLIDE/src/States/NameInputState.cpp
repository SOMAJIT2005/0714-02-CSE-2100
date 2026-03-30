#include "States/NameInputState.hpp"
#include "States/PlayState.hpp"
#include "Core/GameEngine.hpp"
#include "Systems/InputHandler.hpp"

void NameInputState::handleInput(GameEngine& engine, const SDL_Event& event) {
    GameState& data = engine.getGameData();

    if (InputHandler::isBackspace(event)) {
        if (!data.playerNames[data.currentNameInput].empty()) {
            data.playerNames[data.currentNameInput].pop_back();
        }
    } else if (InputHandler::isEnter(event)) {
        if (!data.playerNames[data.currentNameInput].empty()) {
            if (data.currentNameInput == 0) {
                data.currentNameInput = 1;
            } else {
                SDL_StopTextInput();
                engine.changeState(std::make_unique<PlayState>());
            }
        }
    }

    char c;
    if (InputHandler::isAlphaNumericInput(event, c)) {
        auto &s = data.playerNames[data.currentNameInput];
        if (s.size() < MAX_NAME_LENGTH - 1) {
            s += c;
        }
    }
}

void NameInputState::update(GameEngine& engine, Uint32 deltaMs) {}

void NameInputState::render(GameEngine& engine) {
    Renderer& renderer = engine.getRenderer();
    AppContext& ctx = engine.getContext();
    GameState& data = engine.getGameData();

    renderer.drawTextCentered(ctx.fontLarge, "Enter Player Names", 150, {100, 50, 150, 255});
    
    char prompt[64];
    snprintf(prompt, sizeof(prompt), "Enter Player %d Name:", data.currentNameInput + 1);
    renderer.drawTextCentered(ctx.fontRegular, prompt, 250, {50, 50, 50, 255});

    SDL_Rect input_box_rect = {100, 300, WINDOW_WIDTH - 200, 50};
    renderer.drawRect(input_box_rect, {160, 130, 200, 255}, false);

    const std::string& currentName = data.playerNames[data.currentNameInput];
    if (!currentName.empty()) {
        renderer.drawText(ctx.fontRegular, currentName, input_box_rect.x + 10, input_box_rect.y + 10, {0, 0, 0, 255});
    }

    renderer.drawTextCentered(ctx.fontRegular, "Press ENTER to confirm", 400, {50, 50, 50, 255});
}