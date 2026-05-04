#include "States/NameInputState.hpp"
#include "States/PlayState.hpp"
#include "Core/GameEngine.hpp"
#include "Systems/InputHandler.hpp"

void NameInputState::handleInput(GameEngine& engine, const SDL_Event& event) {
    GameState& data = engine.getGameData();
    
    // Defaulting to Player 1 for setup, adapt if setting both names!
    int currentPlayer = 0; 

    if (InputHandler::isBackspace(event)) {
        if (!data.playerNames[currentPlayer].empty()) {
            data.playerNames[currentPlayer].pop_back();
        }
    } else if (InputHandler::isEnter(event)) {
        if (!data.playerNames[currentPlayer].empty()) {
            SDL_StopTextInput();
            // All setup is done, boot into the MVC PlayState!
            engine.changeState(std::make_unique<PlayState>());
        }
    }

    char c;
    if (InputHandler::isAlphaNumericInput(event, c) && data.playerNames[currentPlayer].size() < MAX_NAME_LENGTH) {
        data.playerNames[currentPlayer] += c;
    }
}

void NameInputState::update(GameEngine& engine, Uint32 deltaMs) {}

void NameInputState::render(GameEngine& engine) {
    Renderer& renderer = engine.getRenderer();
    AppContext& ctx = engine.getContext();
    GameState& data = engine.getGameData();

    renderer.drawTextCentered(ctx.fontLarge, "Enter Name", 150, {100, 50, 150, 255});
    
    SDL_Rect input_box_rect = {100, 300, WINDOW_WIDTH - 200, 50};
    renderer.drawRect(input_box_rect, {160, 130, 200, 255}, false);

    if (!data.playerNames[0].empty()) {
        renderer.drawText(ctx.fontRegular, data.playerNames[0], input_box_rect.x + 10, input_box_rect.y + 10, {0, 0, 0, 255});
    }
    
    renderer.drawTextCentered(ctx.fontRegular, "Press ENTER to Start", 400, {50, 50, 50, 255});
}