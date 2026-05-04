#include "MVC/PlayView.hpp"
#include <cstdio> 

PlayView::PlayView(PlayModel* m, Renderer& r, Audio& a, AppContext& c)
    : model(m), renderer(r), audio(a), ctx(c) {}

void PlayView::initUI() {
    giveUpBtn = std::make_shared<Button>(
        SDL_Rect{WINDOW_WIDTH - 130, WINDOW_HEIGHT - 45, 120, 35}, 
        "Give Up", 
        ctx.fontRegular
    );
}

std::shared_ptr<Button> PlayView::getGiveUpButton() const {
    return giveUpBtn;
}

void PlayView::draw() {
    GameState& data = model->getData(); 

    char timerText[32];
    Uint32 remaining = GAME_DURATION_MS > (SDL_GetTicks() - data.gameStartTime) ? GAME_DURATION_MS - (SDL_GetTicks() - data.gameStartTime) : 0;
    snprintf(timerText, sizeof(timerText), "Time: %02d:%02d", (remaining / 1000) / 60, (remaining / 1000) % 60);
    renderer.drawText(ctx.fontRegular, timerText, WINDOW_WIDTH - 150, 10, {100, 50, 150, 255});

    // Draw the Grid Lines perfectly straight
    for (int i = 0; i <= GRID_SIZE; ++i) {
        renderer.drawLine(i * TILE_SIZE, 0, i * TILE_SIZE, WINDOW_HEIGHT, {160, 130, 200, 255});
        renderer.drawLine(0, i * TILE_SIZE, WINDOW_WIDTH, i * TILE_SIZE, {160, 130, 200, 255});
    }

    // Draw Placed Letters
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            char c = data.grid[i][j];
            if (c) {
                char letter[2] = { c, '\0' };
                renderer.drawText(ctx.fontRegular, letter, i * TILE_SIZE + 10, j * TILE_SIZE + 10, {0, 0, 0, 255});
            }
        }
    }

    // Draw Highlight / Typing Letter
    if (data.isTileSelected) {
        SDL_Rect rect = { data.selectedX * TILE_SIZE, data.selectedY * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        renderer.drawRect(rect, {150, 150, 150, 100}, true); 
        if (data.currentLetter) {
            char letter[2] = { data.currentLetter, '\0' };
            renderer.drawText(ctx.fontRegular, letter, data.selectedX * TILE_SIZE + 10, data.selectedY * TILE_SIZE + 10, {0, 0, 0, 255});
        }
    }

    char scoreText[256];
    snprintf(scoreText, sizeof(scoreText), "P1: %d | P2: %d", data.scores[0], data.scores[1]);
    renderer.drawText(ctx.fontRegular, scoreText, 10, WINDOW_HEIGHT - 40, {0, 0, 0, 255});

    if (giveUpBtn) {
        giveUpBtn->render(renderer);
    }
}