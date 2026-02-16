#include "../include/graphics.h"
#include <string.h>
#include <stdio.h>

static void Graphics_DrawText(AppContext *app, TTF_Font *fontToUse, const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(fontToUse, text, color);
    if (surface) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(app->renderer, surface);
        SDL_Rect rect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(app->renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }
}

static void Graphics_DrawTextCentered(AppContext *app, TTF_Font *fontToUse, const char *text, int y, SDL_Color color) {
    int textWidth = 0, textHeight = 0;
    TTF_SizeText(fontToUse, text, &textWidth, &textHeight);
    int x = (WINDOW_WIDTH - textWidth) / 2;
    Graphics_DrawText(app, fontToUse, text, x, y, color);
}

static void Graphics_DrawButton(AppContext *app, Button *button) {
    SDL_Color btnColor = {100, 50, 150, 255}; 
    SDL_Color hoverColor = {160, 130, 200, 255}; 
    SDL_Color textColor = {255, 255, 255, 255}; 

    if (button->isHovered) {
        SDL_SetRenderDrawColor(app->renderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
    } else {
        SDL_SetRenderDrawColor(app->renderer, btnColor.r, btnColor.g, btnColor.b, btnColor.a);
    }
    
    SDL_RenderFillRect(app->renderer, &button->rect);

    int textWidth = 0, textHeight = 0;
    TTF_SizeText(app->fontRegular, button->text, &textWidth, &textHeight);
    int textX = button->rect.x + (button->rect.w - textWidth) / 2;
    int textY = button->rect.y + (button->rect.h - textHeight) / 2;
    Graphics_DrawText(app, app->fontRegular, button->text, textX, textY, textColor);
}

void Graphics_RenderSplashScreen(AppContext *app, GameState *game) {
    SDL_SetRenderDrawColor(app->renderer, 230, 210, 255, 255);
    SDL_RenderClear(app->renderer);
    SDL_Color titleColor = {100, 50, 150, 255}; 
    Graphics_DrawTextCentered(app, app->fontLarge, "WORDS_COLLIDE", 250, titleColor);
    Graphics_DrawButton(app, &game->startGameButton);
    SDL_RenderPresent(app->renderer);
}

void Graphics_RenderNameInput(AppContext *app, GameState *game) {
    SDL_SetRenderDrawColor(app->renderer, 230, 210, 255, 255);
    SDL_RenderClear(app->renderer);
    
    SDL_Color titleColor = {100, 50, 150, 255}; 
    SDL_Color boxColor = {160, 130, 200, 255};   
    SDL_Color textColor = {0, 0, 0, 255};         
    SDL_Color promptColor = {50, 50, 50, 255};  
    
    Graphics_DrawTextCentered(app, app->fontLarge, "Enter Player Names", 150, titleColor);
    
    char prompt[50];
    sprintf(prompt, "Enter Player %d Name:", game->currentNameInput + 1);
    Graphics_DrawTextCentered(app, app->fontRegular, prompt, 250, promptColor);
    
    SDL_Rect input_box_rect = { 100, 300, WINDOW_WIDTH - 200, 50 };
    SDL_SetRenderDrawColor(app->renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
    SDL_RenderDrawRect(app->renderer, &input_box_rect);
    
    const char *currentName = game->playerNames[game->currentNameInput];
    int textWidth = 0, textHeight = 0;
    if (strlen(currentName) > 0) {
        TTF_SizeText(app->fontRegular, currentName, &textWidth, &textHeight);
        Graphics_DrawText(app, app->fontRegular, currentName, input_box_rect.x + 10, input_box_rect.y + 10, textColor);
    }
    
    if ((SDL_GetTicks() / 500) % 2 == 0) {
        SDL_Rect cursor_rect = { input_box_rect.x + 10 + textWidth + 2, input_box_rect.y + 8, 3, 34 };
        SDL_SetRenderDrawColor(app->renderer, textColor.r, textColor.g, textColor.b, textColor.a);
        SDL_RenderFillRect(app->renderer, &cursor_rect);
    }
    
    Graphics_DrawTextCentered(app, app->fontRegular, "Press ENTER to confirm", 400, promptColor);
    SDL_RenderPresent(app->renderer);
}

void Graphics_RenderGame(AppContext *app, GameState *game) {
    SDL_SetRenderDrawColor(app->renderer, 230, 210, 255, 255);
    SDL_RenderClear(app->renderer);
    
    // Timer
    SDL_Color timerColor = {100, 50, 150, 255};
    Uint32 elapsedTime = SDL_GetTicks() - game->gameStartTime;
    Uint32 timeLeft = GAME_DURATION_MS - elapsedTime;
    if (elapsedTime >= GAME_DURATION_MS) timeLeft = 0;
    
    int totalSeconds = timeLeft / 1000;
    char timerText[20];
    sprintf(timerText, "Time: %02d:%02d", totalSeconds / 60, totalSeconds % 60);
    if (timeLeft < WARNING_TIME_MS && timeLeft > 0 && (SDL_GetTicks() / 250) % 2 == 0) { 
        timerColor = (SDL_Color){255, 0, 0, 255};
    }
    
    int tw = 0, th = 0;
    TTF_SizeText(app->fontRegular, timerText, &tw, &th);
    Graphics_DrawText(app, app->fontRegular, timerText, WINDOW_WIDTH - tw - 10, 10, timerColor);

    // Turn Timer Bar
    Uint32 turnElapsed = SDL_GetTicks() - game->turnStartTime;
    float ratio = 1.0f - ((float)turnElapsed / TURN_DURATION_MS);
    if (ratio < 0) ratio = 0;

    SDL_Rect barBg = { 10, 10, 200, 15 };
    SDL_Rect barFg = { 10, 10, (int)(200 * ratio), 15 };
    
    SDL_SetRenderDrawColor(app->renderer, 200, 0, 0, 255); 
    SDL_RenderFillRect(app->renderer, &barBg);
    SDL_SetRenderDrawColor(app->renderer, 0, 200, 0, 255); 
    SDL_RenderFillRect(app->renderer, &barFg);
    Graphics_DrawText(app, app->fontRegular, "Turn Time", 10, 30, (SDL_Color){50, 50, 50, 255});

    // Grid
    SDL_SetRenderDrawColor(app->renderer, 160, 130, 200, 255);
    for (int i = 0; i <= GRID_SIZE; i++) {
        SDL_RenderDrawLine(app->renderer, i * TILE_SIZE, 0, i * TILE_SIZE, WINDOW_HEIGHT);
        SDL_RenderDrawLine(app->renderer, 0, i * TILE_SIZE, WINDOW_WIDTH, i * TILE_SIZE);
    }
    
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (game->grid[i][j]) {
                char letter[2] = {game->grid[i][j], '\0'};
                Graphics_DrawText(app, app->fontRegular, letter, i * TILE_SIZE + 10, j * TILE_SIZE + 10, (SDL_Color){0,0,0,255});
            }
        }
    }
    
    // Selection
    if (game->isTileSelected) {
        SDL_SetRenderDrawColor(app->renderer, 150, 150, 150, 100);
        SDL_Rect rect = {game->selectedX * TILE_SIZE, game->selectedY * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        SDL_RenderFillRect(app->renderer, &rect);
        
        if (game->currentLetter) {
            char letter[2] = {game->currentLetter, '\0'};
            Graphics_DrawText(app, app->fontRegular, letter, game->selectedX * TILE_SIZE + 10, game->selectedY * TILE_SIZE + 10, (SDL_Color){0,0,0,255});
        }
    }
    
    // Scores
    char scoreText[200];
    const char *p1Name = (strlen(game->playerNames[0]) > 0) ? game->playerNames[0] : "Player 1";
    const char *p2Name = (strlen(game->playerNames[1]) > 0) ? game->playerNames[1] : "Player 2";
    const char *turnName = (game->currentPlayer == 0) ? p1Name : p2Name;
    sprintf(scoreText, "%s: %d  |  %s: %d  |  Turn: %s", p1Name, game->scores[0], p2Name, game->scores[1], turnName);
    
    Graphics_DrawText(app, app->fontRegular, scoreText, 10, WINDOW_HEIGHT - 40, (SDL_Color){0, 0, 0, 255});
    Graphics_DrawButton(app, &game->giveUpButton);
    SDL_RenderPresent(app->renderer);
}

void Graphics_RenderGameOver(AppContext *app, GameState *game) {
    SDL_SetRenderDrawColor(app->renderer, 230, 210, 255, 255);
    SDL_RenderClear(app->renderer);
    
    Uint32 elapsedTime = SDL_GetTicks() - game->gameStartTime;
    if (elapsedTime >= GAME_DURATION_MS) {
        Graphics_DrawTextCentered(app, app->fontLarge, "Time's Up!", 150, (SDL_Color){100, 50, 150, 255});
    } else {
        Graphics_DrawTextCentered(app, app->fontLarge, "Game Over", 150, (SDL_Color){100, 50, 150, 255});
    }
    
    const char *p1Name = (strlen(game->playerNames[0]) > 0) ? game->playerNames[0] : "Player 1";
    const char *p2Name = (strlen(game->playerNames[1]) > 0) ? game->playerNames[1] : "Player 2";
    
    char p1Score[100], p2Score[100];
    sprintf(p1Score, "%s's Score: %d", p1Name, game->scores[0]);
    sprintf(p2Score, "%s's Score: %d", p2Name, game->scores[1]);
    
    Graphics_DrawTextCentered(app, app->fontRegular, p1Score, 250, (SDL_Color){0, 0, 0, 255});
    Graphics_DrawTextCentered(app, app->fontRegular, p2Score, 300, (SDL_Color){0, 0, 0, 255});
    
    char winnerText[100];
    if (game->scores[0] > game->scores[1]) sprintf(winnerText, "%s wins!", p1Name);
    else if (game->scores[1] > game->scores[0]) sprintf(winnerText, "%s wins!", p2Name);
    else sprintf(winnerText, "It's a tie!");
    
    Graphics_DrawTextCentered(app, app->fontLarge, winnerText, 400, (SDL_Color){0, 100, 0, 255});
    Graphics_DrawButton(app, &game->playAgainButton);
    Graphics_DrawTextCentered(app, app->fontRegular, "Press any key to exit", 550, (SDL_Color){50, 50, 50, 255});
    SDL_RenderPresent(app->renderer);
}