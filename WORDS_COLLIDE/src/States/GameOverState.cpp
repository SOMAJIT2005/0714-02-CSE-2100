#include "States/GameOverState.hpp"
#include "States/SplashState.hpp"
#include "Core/GameEngine.hpp"
#include "Systems/InputHandler.hpp"

void GameOverState::handleInput(GameEngine& engine, const SDL_Event& event) {
    // The new Button class handles hover states and clicks automatically!
    if (playAgainBtn && playAgainBtn->handleInput(event)) {
        // If clicked, restart the game (assuming you have a PlayState transition here)
        engine.getAudio().playSound(engine.getContext().sfxValidWord); // Optional sound
        // engine.changeState(std::make_unique<PlayState>()); // Uncomment or adapt to your logic
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

    if (playAgainBtn) {
        playAgainBtn->render(engine.getRenderer());
    }
}

void GameOverState::onEnter(GameEngine& engine) {
    // Create the button (adjust the X and Y coordinates as needed for your screen size)
    playAgainBtn = std::make_shared<Button>(
        SDL_Rect{WINDOW_WIDTH / 2 - 75, WINDOW_HEIGHT / 2 + 50, 150, 40}, 
        "Play Again", 
        engine.getContext().fontRegular
    );
}