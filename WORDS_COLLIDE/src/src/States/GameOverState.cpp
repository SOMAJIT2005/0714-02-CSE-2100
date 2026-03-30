#include "States/GameOverState.hpp"
#include "States/SplashState.hpp"
#include "Core/GameEngine.hpp"
#include "Systems/InputHandler.hpp"

void GameOverState::handleInput(GameEngine& engine, const SDL_Event& event) {
    GameState& data = engine.getGameData();

    data.playAgainButton.isHovered = InputHandler::isMouseOverButton(event, data.playAgainButton);

    if (InputHandler::isMouseClickOnButton(event, data.playAgainButton)) {
        // Reset necessary game data here if needed, then back to menu
        engine.changeState(std::make_unique<SplashState>());
    }
}

void GameOverState::update(GameEngine& engine, Uint32 deltaMs) {}

void GameOverState::render(GameEngine& engine) {
    Renderer& renderer = engine.getRenderer();
    AppContext& ctx = engine.getContext();
    GameState& data = engine.getGameData();

    renderer.drawTextCentered(ctx.fontLarge, "Game Over", 150, {100, 50, 150, 255});

    char p1Score[128], p2Score[128];
    snprintf(p1Score, sizeof(p1Score), "%s's Score: %d", data.playerNames[0].c_str(), data.scores[0]);
    snprintf(p2Score, sizeof(p2Score), "%s's Score: %d", data.playerNames[1].c_str(), data.scores[1]);

    renderer.drawTextCentered(ctx.fontRegular, p1Score, 250, {0, 0, 0, 255});
    renderer.drawTextCentered(ctx.fontRegular, p2Score, 300, {0, 0, 0, 255});

    renderer.drawButton(data.playAgainButton);
}