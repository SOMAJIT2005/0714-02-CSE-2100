#include "Graphics.hpp"
#include <SDL2/SDL_ttf.h>
#include <cstring>

Graphics::Graphics(AppContext& ctx) : app(ctx) {}

void Graphics::drawText(TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_Rect rect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(app.renderer, texture, nullptr, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void Graphics::drawTextCentered(TTF_Font* font, const std::string& text, int y, SDL_Color color) {
    int w = 0, h = 0;
    TTF_SizeText(font, text.c_str(), &w, &h);
    int x = (WINDOW_WIDTH - w) / 2;
    drawText(font, text, x, y, color);
}

void Graphics::drawButton(Button& button) {
    SDL_Color btnColor = {100,50,150,255};
    SDL_Color hoverColor = {160,130,200,255};
    SDL_Color textColor = {255,255,255,255};
    SDL_Color use = button.isHovered ? hoverColor : btnColor;
    SDL_SetRenderDrawColor(app.renderer, use.r, use.g, use.b, use.a);
    SDL_RenderFillRect(app.renderer, &button.rect);

    // Center text inside button
    int textW = 0, textH = 0;
    if (app.fontRegular && !button.text.empty()) {
        TTF_SizeText(app.fontRegular, button.text.c_str(), &textW, &textH);
    }
    int textX = button.rect.x + (button.rect.w - textW) / 2;
    int textY = button.rect.y + (button.rect.h - textH) / 2;
    drawText(app.fontRegular, button.text, textX, textY, textColor);
}

void Graphics::renderSplash(GameState& game) {
    SDL_SetRenderDrawColor(app.renderer, 230,210,255,255);
    SDL_RenderClear(app.renderer);
    SDL_Color titleColor = {100,50,150,255};
    drawTextCentered(app.fontLarge, "WORDS_COLLIDE", 200, titleColor);
    drawButton(game.startGameButton);
    drawButton(game.multiplayerButton);
    SDL_RenderPresent(app.renderer);
}

void Graphics::renderMultiplayerMenu(GameState& game) {
    SDL_SetRenderDrawColor(app.renderer, 230,210,255,255);
    SDL_RenderClear(app.renderer);
    SDL_Color titleColor = {100,50,150,255};
    drawTextCentered(app.fontLarge, "Multiplayer Setup", 150, titleColor);
    drawTextCentered(app.fontRegular, "Make sure both devices are on the same Wi-Fi!", 250, {50,50,50,255});
    drawButton(game.hostGameButton);
    drawButton(game.joinGameButton);
    drawButton(game.backButton);
    SDL_RenderPresent(app.renderer);
}

void Graphics::renderIPInput(GameState& game) {
    SDL_SetRenderDrawColor(app.renderer, 230,210,255,255);
    SDL_RenderClear(app.renderer);
    SDL_Color titleColor = {100,50,150,255};
    SDL_Color boxColor = {160,130,200,255};
    SDL_Color textColor = {0,0,0,255};
    SDL_Color promptColor = {50,50,50,255};

    drawTextCentered(app.fontLarge, "Join Game", 150, titleColor);
    drawTextCentered(app.fontRegular, "Enter the Host's IP Address:", 250, promptColor);

    SDL_Rect input_box_rect = {100, 300, WINDOW_WIDTH - 200, 50};
    SDL_SetRenderDrawColor(app.renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
    SDL_RenderDrawRect(app.renderer, &input_box_rect);

    if (!game.targetIP.empty()) {
        drawText(app.fontRegular, game.targetIP, input_box_rect.x + 10, input_box_rect.y + 10, textColor);
    }

    if ((SDL_GetTicks() / 500) % 2 == 0) {
        int textWidth = 0, textHeight = 0;
        if (!game.targetIP.empty()) TTF_SizeText(app.fontRegular, game.targetIP.c_str(), &textWidth, &textHeight);
        SDL_Rect cursor_rect = { input_box_rect.x + 10 + textWidth + 2, input_box_rect.y + 8, 3, 34 };
        SDL_SetRenderDrawColor(app.renderer, textColor.r, textColor.g, textColor.b, textColor.a);
        SDL_RenderFillRect(app.renderer, &cursor_rect);
    }

    drawTextCentered(app.fontRegular, "Press ENTER to Connect", 400, promptColor);
    drawButton(game.backButton);
    SDL_RenderPresent(app.renderer);
}

void Graphics::renderNameInput(GameState& game) {
    SDL_SetRenderDrawColor(app.renderer, 230,210,255,255);
    SDL_RenderClear(app.renderer);
    SDL_Color titleColor = {100,50,150,255};
    SDL_Color boxColor = {160,130,200,255};
    SDL_Color textColor = {0,0,0,255};
    SDL_Color promptColor = {50,50,50,255};

    drawTextCentered(app.fontLarge, "Enter Player Names", 150, titleColor);
    char prompt[64];
    snprintf(prompt, sizeof(prompt), "Enter Player %d Name:", game.currentNameInput + 1);
    drawTextCentered(app.fontRegular, prompt, 250, promptColor);

    SDL_Rect input_box_rect = {100, 300, WINDOW_WIDTH - 200, 50};
    SDL_SetRenderDrawColor(app.renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
    SDL_RenderDrawRect(app.renderer, &input_box_rect);

    const std::string& currentName = game.playerNames[game.currentNameInput];
    if (!currentName.empty()) {
        drawText(app.fontRegular, currentName, input_box_rect.x + 10, input_box_rect.y + 10, textColor);
    }

    if ((SDL_GetTicks() / 500) % 2 == 0) {
        int textWidth = 0, textHeight = 0;
        if (!currentName.empty()) TTF_SizeText(app.fontRegular, currentName.c_str(), &textWidth, &textHeight);
        SDL_Rect cursor_rect = { input_box_rect.x + 10 + textWidth + 2, input_box_rect.y + 8, 3, 34 };
        SDL_SetRenderDrawColor(app.renderer, textColor.r, textColor.g, textColor.b, textColor.a);
        SDL_RenderFillRect(app.renderer, &cursor_rect);
    }

    drawTextCentered(app.fontRegular, "Press ENTER to confirm", 400, promptColor);
    SDL_RenderPresent(app.renderer);
}

void Graphics::renderGame(GameState& game) {
    SDL_SetRenderDrawColor(app.renderer, 230,210,255,255);
    SDL_RenderClear(app.renderer);

    SDL_Color timerColor = {100,50,150,255};
    Uint32 elapsedTime = SDL_GetTicks() - game.gameStartTime;
    Uint32 timeLeft = GAME_DURATION_MS > elapsedTime ? GAME_DURATION_MS - elapsedTime : 0;

    int totalSeconds = timeLeft / 1000;
    char timerText[32];
    snprintf(timerText, sizeof(timerText), "Time: %02d:%02d", totalSeconds / 60, totalSeconds % 60);
    if (timeLeft < WARNING_TIME_MS && timeLeft > 0 && (SDL_GetTicks() / 250) % 2 == 0) {
        timerColor = {255,0,0,255};
    }
    int tw = 0, th = 0;
    TTF_SizeText(app.fontRegular, timerText, &tw, &th);
    drawText(app.fontRegular, timerText, WINDOW_WIDTH - tw - 10, 10, timerColor);

    Uint32 turnElapsed = SDL_GetTicks() - game.turnStartTime;
    float ratio = 1.0f - (float(turnElapsed) / float(TURN_DURATION_MS));
    if (ratio < 0) ratio = 0.0f;

    SDL_Rect barBg = {10, 10, 200, 15};
    SDL_Rect barFg = {10, 10, int(200 * ratio), 15};
    SDL_SetRenderDrawColor(app.renderer, 200,0,0,255);
    SDL_RenderFillRect(app.renderer, &barBg);
    SDL_SetRenderDrawColor(app.renderer, 0,200,0,255);
    SDL_RenderFillRect(app.renderer, &barFg);
    drawText(app.fontRegular, "Turn Time", 10, 30, {50,50,50,255});

    SDL_SetRenderDrawColor(app.renderer, 160,130,200,255);
    for (int i = 0; i <= GRID_SIZE; ++i) {
        SDL_RenderDrawLine(app.renderer, i * TILE_SIZE, 0, i * TILE_SIZE, WINDOW_HEIGHT);
        SDL_RenderDrawLine(app.renderer, 0, i * TILE_SIZE, WINDOW_WIDTH, i * TILE_SIZE);
    }

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            char c = game.grid[i][j];
            if (c) {
                char letter[2] = { c, '\0' };
                drawText(app.fontRegular, letter, i * TILE_SIZE + 10, j * TILE_SIZE + 10, {0,0,0,255});
            }
        }
    }

    if (game.isTileSelected) {
        SDL_SetRenderDrawColor(app.renderer, 150,150,150,100);
        SDL_Rect rect = { game.selectedX * TILE_SIZE, game.selectedY * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        SDL_RenderFillRect(app.renderer, &rect);
        if (game.currentLetter) {
            char letter[2] = { game.currentLetter, '\0' };
            drawText(app.fontRegular, letter, game.selectedX * TILE_SIZE + 10, game.selectedY * TILE_SIZE + 10, {0,0,0,255});
        }
    }

    char scoreText[256];
    const std::string& p1Name = !game.playerNames[0].empty() ? game.playerNames[0] : "Player 1";
    const std::string& p2Name = !game.playerNames[1].empty() ? game.playerNames[1] : "Player 2";
    const std::string& turnName = game.currentPlayer == 0 ? p1Name : p2Name;
    snprintf(scoreText, sizeof(scoreText), "%s: %d  |  %s: %d  |  Turn: %s", p1Name.c_str(), game.scores[0], p2Name.c_str(), game.scores[1], turnName.c_str());
    drawText(app.fontRegular, scoreText, 10, WINDOW_HEIGHT - 40, {0,0,0,255});
    drawButton(game.giveUpButton);
    SDL_RenderPresent(app.renderer);
}

void Graphics::renderGameOver(GameState& game) {
    SDL_SetRenderDrawColor(app.renderer, 230,210,255,255);
    SDL_RenderClear(app.renderer);

    Uint32 elapsedTime = SDL_GetTicks() - game.gameStartTime;
    if (elapsedTime >= GAME_DURATION_MS) {
        drawTextCentered(app.fontLarge, "Time's Up!", 150, {100,50,150,255});
    } else {
        drawTextCentered(app.fontLarge, "Game Over", 150, {100,50,150,255});
    }

    const std::string& p1Name = !game.playerNames[0].empty() ? game.playerNames[0] : "Player 1";
    const std::string& p2Name = !game.playerNames[1].empty() ? game.playerNames[1] : "Player 2";

    char p1Score[128], p2Score[128];
    snprintf(p1Score, sizeof(p1Score), "%s's Score: %d", p1Name.c_str(), game.scores[0]);
    snprintf(p2Score, sizeof(p2Score), "%s's Score: %d", p2Name.c_str(), game.scores[1]);

    drawTextCentered(app.fontRegular, p1Score, 250, {0,0,0,255});
    drawTextCentered(app.fontRegular, p2Score, 300, {0,0,0,255});

    char winnerText[128];
    if (game.scores[0] > game.scores[1]) snprintf(winnerText, sizeof(winnerText), "%s wins!", p1Name.c_str());
    else if (game.scores[1] > game.scores[0]) snprintf(winnerText, sizeof(winnerText), "%s wins!", p2Name.c_str());
    else snprintf(winnerText, sizeof(winnerText), "It's a tie!");

    drawTextCentered(app.fontLarge, winnerText, 400, {0,100,0,255});
    drawButton(game.playAgainButton);
    drawTextCentered(app.fontRegular, "Press any key to exit", 550, {50,50,50,255});
    SDL_RenderPresent(app.renderer);
}
