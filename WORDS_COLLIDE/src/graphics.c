#include "../include/graphics.h"
#include <string.h>
#include <stdio.h>

static void Graphics_DrawText(AppContext *app, TTF_Font *fontToUse, const char *text, int xPosition, int yPosition, SDL_Color color) {
    SDL_Surface *textSurface = TTF_RenderText_Solid(fontToUse, text, color);
    if (textSurface) {
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(app->renderer, textSurface);
        SDL_Rect renderQuad = {xPosition, yPosition, textSurface->w, textSurface->h};
        
        SDL_RenderCopy(app->renderer, textTexture, NULL, &renderQuad);
        
        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface(textSurface);
    }
}

static void Graphics_DrawTextCentered(AppContext *app, TTF_Font *fontToUse, const char *text, int yPosition, SDL_Color color) {
    int textWidth = 0, textHeight = 0;
    TTF_SizeText(fontToUse, text, &textWidth, &textHeight);
    
    int centeredXPosition = (WINDOW_WIDTH - textWidth) / 2;
    Graphics_DrawText(app, fontToUse, text, centeredXPosition, yPosition, color);
}

static void Graphics_DrawButton(AppContext *app, Button *button) {
    SDL_Color defaultColor = {100, 50, 150, 255}; 
    SDL_Color hoverColor   = {160, 130, 200, 255}; 
    SDL_Color textColor    = {255, 255, 255, 255}; 

    if (button->isHovered) {
        SDL_SetRenderDrawColor(app->renderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
    } else {
        SDL_SetRenderDrawColor(app->renderer, defaultColor.r, defaultColor.g, defaultColor.b, defaultColor.a);
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
    
    SDL_Color titleColor  = {100, 50, 150, 255}; 
    SDL_Color boxColor    = {160, 130, 200, 255};   
    SDL_Color textColor   = {0, 0, 0, 255};         
    SDL_Color promptColor = {50, 50, 50, 255};  
    
    Graphics_DrawTextCentered(app, app->fontLarge, "Enter Player Names", 150, titleColor);
    
    char promptMessage[50];
    sprintf(promptMessage, "Enter Player %d Name:", game->currentNameInput + 1);
    Graphics_DrawTextCentered(app, app->fontRegular, promptMessage, 250, promptColor);
    
    SDL_Rect inputBoxRect = { 100, 300, WINDOW_WIDTH - 200, 50 };
    SDL_SetRenderDrawColor(app->renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
    SDL_RenderDrawRect(app->renderer, &inputBoxRect);
    
    const char *currentName = game->playerNames[game->currentNameInput];
    int textWidth = 0, textHeight = 0;
    
    if (strlen(currentName) > 0) {
        TTF_SizeText(app->fontRegular, currentName, &textWidth, &textHeight);
        Graphics_DrawText(app, app->fontRegular, currentName, inputBoxRect.x + 10, inputBoxRect.y + 10, textColor);
    }
    
    // Blinking cursor logic
    if ((SDL_GetTicks() / 500) % 2 == 0) {
        SDL_Rect cursorRect = { inputBoxRect.x + 10 + textWidth + 2, inputBoxRect.y + 8, 3, 34 };
        SDL_SetRenderDrawColor(app->renderer, textColor.r, textColor.g, textColor.b, textColor.a);
        SDL_RenderFillRect(app->renderer, &cursorRect);
    }
    
    Graphics_DrawTextCentered(app, app->fontRegular, "Press ENTER to confirm", 400, promptColor);
    SDL_RenderPresent(app->renderer);
}

void Graphics_RenderGame(AppContext *app, GameState *game) {
    SDL_SetRenderDrawColor(app->renderer, 230, 210, 255, 255);
    SDL_RenderClear(app->renderer);
    
    // Calculate Game Timer
    SDL_Color timerColor = {100, 50, 150, 255};
    Uint32 timeSinceStart = SDL_GetTicks() - game->gameStartTime;
    Uint32 timeRemaining = GAME_DURATION_MS > timeSinceStart ? GAME_DURATION_MS - timeSinceStart : 0;
    
    int totalSecondsLeft = timeRemaining / 1000;
    int minutesLeft = totalSecondsLeft / 60;
    int secondsLeft = totalSecondsLeft % 60;
    
    char timerString[20];
    sprintf(timerString, "Time: %02d:%02d", minutesLeft, secondsLeft);
    
    // Make timer flash red if running low
    if (timeRemaining < WARNING_TIME_MS && timeRemaining > 0 && (SDL_GetTicks() / 250) % 2 == 0) { 
        timerColor = (SDL_Color){255, 0, 0, 255};
    }
    
    int timerWidth = 0, timerHeight = 0;
    TTF_SizeText(app->fontRegular, timerString, &timerWidth, &timerHeight);
    Graphics_DrawText(app, app->fontRegular, timerString, WINDOW_WIDTH - timerWidth - 10, 10, timerColor);

    // Calculate and Draw Turn Timer Progress Bar
    Uint32 timeSinceTurnStart = SDL_GetTicks() - game->turnStartTime;
    float timeRemainingPercentage = 1.0f - ((float)timeSinceTurnStart / TURN_DURATION_MS);
    if (timeRemainingPercentage < 0) timeRemainingPercentage = 0;

    SDL_Rect progressBarBackground = { 10, 10, 200, 15 };
    SDL_Rect progressBarForeground = { 10, 10, (int)(200 * timeRemainingPercentage), 15 };
    
    SDL_SetRenderDrawColor(app->renderer, 200, 0, 0, 255); // Red Background
    SDL_RenderFillRect(app->renderer, &progressBarBackground);
    
    SDL_SetRenderDrawColor(app->renderer, 0, 200, 0, 255); // Green Foreground
    SDL_RenderFillRect(app->renderer, &progressBarForeground);
    
    Graphics_DrawText(app, app->fontRegular, "Turn Time", 10, 30, (SDL_Color){50, 50, 50, 255});

    // Draw Grid Lines
    SDL_SetRenderDrawColor(app->renderer, 160, 130, 200, 255);
    for (int line = 0; line <= GRID_SIZE; line++) {
        SDL_RenderDrawLine(app->renderer, line * TILE_SIZE, 0, line * TILE_SIZE, WINDOW_HEIGHT);
        SDL_RenderDrawLine(app->renderer, 0, line * TILE_SIZE, WINDOW_WIDTH, line * TILE_SIZE);
    }
    
    // Draw Placed Tiles
    for (int col = 0; col < GRID_SIZE; col++) {
        for (int row = 0; row < GRID_SIZE; row++) {
            if (game->grid[col][row] != '\0') {
                char letterString[2] = {game->grid[col][row], '\0'};
                Graphics_DrawText(app, app->fontRegular, letterString, col * TILE_SIZE + 10, row * TILE_SIZE + 10, (SDL_Color){0,0,0,255});
            }
        }
    }
    
    // Highlight Selected Tile
    if (game->isTileSelected) {
        SDL_SetRenderDrawColor(app->renderer, 150, 150, 150, 100);
        SDL_Rect highlightRect = {game->selectedX * TILE_SIZE, game->selectedY * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        SDL_RenderFillRect(app->renderer, &highlightRect);
        
        if (game->currentLetter != '\0') {
            char currentLetterString[2] = {game->currentLetter, '\0'};
            Graphics_DrawText(app, app->fontRegular, currentLetterString, game->selectedX * TILE_SIZE + 10, game->selectedY * TILE_SIZE + 10, (SDL_Color){0,0,0,255});
        }
    }
    
    // Draw Scores
    char scoreDisplayString[200];
    const char *player1Name = (strlen(game->playerNames[0]) > 0) ? game->playerNames[0] : "Player 1";
    const char *player2Name = (strlen(game->playerNames[1]) > 0) ? game->playerNames[1] : "Player 2";
    const char *activePlayerName = (game->currentPlayer == 0) ? player1Name : player2Name;
    
    sprintf(scoreDisplayString, "%s: %d  |  %s: %d  |  Turn: %s", player1Name, game->scores[0], player2Name, game->scores[1], activePlayerName);
    
    Graphics_DrawText(app, app->fontRegular, scoreDisplayString, 10, WINDOW_HEIGHT - 40, (SDL_Color){0, 0, 0, 255});
    Graphics_DrawButton(app, &game->giveUpButton);
    SDL_RenderPresent(app->renderer);
}

void Graphics_RenderGameOver(AppContext *app, GameState *game) {
    SDL_SetRenderDrawColor(app->renderer, 230, 210, 255, 255);
    SDL_RenderClear(app->renderer);
    
    Uint32 timeSinceStart = SDL_GetTicks() - game->gameStartTime;
    
    if (timeSinceStart >= GAME_DURATION_MS) {
        Graphics_DrawTextCentered(app, app->fontLarge, "Time's Up!", 150, (SDL_Color){100, 50, 150, 255});
    } else {
        Graphics_DrawTextCentered(app, app->fontLarge, "Game Over", 150, (SDL_Color){100, 50, 150, 255});
    }
    
    const char *player1Name = (strlen(game->playerNames[0]) > 0) ? game->playerNames[0] : "Player 1";
    const char *player2Name = (strlen(game->playerNames[1]) > 0) ? game->playerNames[1] : "Player 2";
    
    char p1ScoreString[100], p2ScoreString[100];
    sprintf(p1ScoreString, "%s's Score: %d", player1Name, game->scores[0]);
    sprintf(p2ScoreString, "%s's Score: %d", player2Name, game->scores[1]);
    
    Graphics_DrawTextCentered(app, app->fontRegular, p1ScoreString, 250, (SDL_Color){0, 0, 0, 255});
    Graphics_DrawTextCentered(app, app->fontRegular, p2ScoreString, 300, (SDL_Color){0, 0, 0, 255});
    
    char winnerAnnouncement[100];
    if (game->scores[0] > game->scores[1]) {
        sprintf(winnerAnnouncement, "%s wins!", player1Name);
    } else if (game->scores[1] > game->scores[0]) {
        sprintf(winnerAnnouncement, "%s wins!", player2Name);
    } else {
        sprintf(winnerAnnouncement, "It's a tie!");
    }
    
    Graphics_DrawTextCentered(app, app->fontLarge, winnerAnnouncement, 400, (SDL_Color){0, 100, 0, 255});
    Graphics_DrawButton(app, &game->playAgainButton);
    Graphics_DrawTextCentered(app, app->fontRegular, "Press any key to exit", 550, (SDL_Color){50, 50, 50, 255});
    
    SDL_RenderPresent(app->renderer);
}