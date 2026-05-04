#include "../include/graphics.h"
#include "../include/network.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

static void Graphics_DrawText(AppContext *app, TTF_Font *fontToUse, const char *text, int x, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(fontToUse, text, color);
    if (surface)
    {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(app->renderer, surface);
        SDL_Rect rect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(app->renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }
}
static void Graphics_DrawTextCentered(AppContext *app, TTF_Font *fontToUse, const char *text, int y, SDL_Color color)
{
    int textWidth = 0, textHeight = 0;
    TTF_SizeText(fontToUse, text, &textWidth, &textHeight);
    int x = (WINDOW_WIDTH - textWidth) / 2;
    Graphics_DrawText(app, fontToUse, text, x, y, color);
}
static void Graphics_DrawAnimatedBackground(AppContext *app)
{
    SDL_SetRenderDrawColor(app->renderer, 240, 230, 255, 255);
    SDL_RenderClear(app->renderer);
    Uint32 time = SDL_GetTicks();
    SDL_Color faintColor = {220, 200, 245, 255};
    for (int i = 0; i < 30; i++)
    {
        char letter[2] = {'A' + (i % 26), '\0'};
        int x = (int)((i * 87) + (time / 40.0)) % (WINDOW_WIDTH + 50) - 25;
        int y = (int)((i * 113) + sin(time / 800.0 + i) * 80) % (WINDOW_HEIGHT + 50) - 25;
        Graphics_DrawText(app, app->fontLarge, letter, x, y, faintColor);
    }
}
static void Graphics_DrawMysteryBackground(AppContext *app)
{
    SDL_SetRenderDrawColor(app->renderer, 15, 10, 35, 255);
    SDL_RenderClear(app->renderer);
    Uint32 time = SDL_GetTicks();
    SDL_Color faintColor = {50, 50, 90, 50};
    for (int i = 0; i < 25; i++)
    {
        char symbol[2] = "?";
        int x = (int)((i * 133) + (time / 70.0)) % (WINDOW_WIDTH + 80) - 40;
        int y = (int)((i * 157) + sin(time / 1500.0 + i) * 50) % (WINDOW_HEIGHT + 80) - 40;
        Graphics_DrawText(app, app->fontLarge, symbol, x, y, faintColor);
    }
}
static void Graphics_DrawButton(AppContext *app, Button *button)
{
    SDL_Color shadowColor = {80, 40, 120, 255};
    SDL_Color btnColor = {110, 60, 160, 255};
    SDL_Color hoverColor = {140, 90, 190, 255};
    SDL_Color textColor = {255, 255, 255, 255};
    int yOffset = button->isHovered ? 3 : 0;
    SDL_Rect shadowRect = {button->rect.x + 3, button->rect.y + 5, button->rect.w, button->rect.h};
    SDL_SetRenderDrawColor(app->renderer, shadowColor.r, shadowColor.g, shadowColor.b, shadowColor.a);
    SDL_RenderFillRect(app->renderer, &shadowRect);
    SDL_Rect visualRect = {button->rect.x, button->rect.y + yOffset, button->rect.w, button->rect.h};
    if (button->isHovered)
        SDL_SetRenderDrawColor(app->renderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
    else
        SDL_SetRenderDrawColor(app->renderer, btnColor.r, btnColor.g, btnColor.b, btnColor.a);
    SDL_RenderFillRect(app->renderer, &visualRect);
    int textWidth = 0, textHeight = 0;
    TTF_SizeText(app->fontRegular, button->text, &textWidth, &textHeight);
    int textX = visualRect.x + (visualRect.w - textWidth) / 2;
    int textY = visualRect.y + (visualRect.h - textHeight) / 2;
    Graphics_DrawText(app, app->fontRegular, button->text, textX, textY, textColor);
}

void Graphics_RenderSplashScreen(AppContext *app, GameState *game)
{
    Graphics_DrawAnimatedBackground(app);
    int titleBob = (int)(sin(SDL_GetTicks() / 300.0) * 8);
    Graphics_DrawTextCentered(app, app->fontLarge, "SELECT GAME", 120 + titleBob, (SDL_Color){90, 30, 140, 255});
    Graphics_DrawButton(app, &game->playWordsCollideButton);
    Graphics_DrawButton(app, &game->playMysteryButton);
}
void Graphics_RenderModeMenu(AppContext *app, GameState *game)
{
    Graphics_DrawAnimatedBackground(app);
    int titleBob = (int)(sin(SDL_GetTicks() / 300.0) * 8);
    Graphics_DrawTextCentered(app, app->fontLarge, "Words Collide: Mode", 120 + titleBob, (SDL_Color){90, 30, 140, 255});
    Graphics_DrawButton(app, &game->classicModeButton);
    Graphics_DrawButton(app, &game->blitzModeButton);
    Graphics_DrawButton(app, &game->backButton);
}
void Graphics_RenderOpponentMenu(AppContext *app, GameState *game)
{
    Graphics_DrawAnimatedBackground(app);
    int titleBob = (int)(sin(SDL_GetTicks() / 300.0) * 8);
    const char *modeText = (game->gameMode == 0) ? "Classic Mode" : "Blitz Mode";
    Graphics_DrawTextCentered(app, app->fontLarge, modeText, 120 + titleBob, (SDL_Color){90, 30, 140, 255});
    Graphics_DrawTextCentered(app, app->fontRegular, "Select Opponent Type", 200 + titleBob, (SDL_Color){60, 60, 60, 255});
    Graphics_DrawButton(app, &game->singlePlayerButton);
    Graphics_DrawButton(app, &game->startGameButton);
    Graphics_DrawButton(app, &game->multiplayerButton);
    Graphics_DrawButton(app, &game->backButton);
}
void Graphics_RenderBotMenu(AppContext *app, GameState *game)
{
    Graphics_DrawAnimatedBackground(app);
    int titleBob = (int)(sin(SDL_GetTicks() / 300.0) * 8);
    Graphics_DrawTextCentered(app, app->fontLarge, "Select Bot Difficulty", 120 + titleBob, (SDL_Color){90, 30, 140, 255});
    Graphics_DrawButton(app, &game->botEasyButton);
    Graphics_DrawButton(app, &game->botHardButton);
    Graphics_DrawButton(app, &game->backButton);
}
void Graphics_RenderMultiplayerMenu(AppContext *app, GameState *game)
{
    Graphics_DrawAnimatedBackground(app);
    int titleBob = (int)(sin(SDL_GetTicks() / 300.0) * 8);
    Graphics_DrawTextCentered(app, app->fontLarge, "Multiplayer Setup", 120 + titleBob, (SDL_Color){90, 30, 140, 255});
    Graphics_DrawTextCentered(app, app->fontRegular, "Make sure both devices are on the same Wi-Fi!", 220 + titleBob, (SDL_Color){60, 60, 60, 255});
    Graphics_DrawButton(app, &game->hostGameButton);
    Graphics_DrawButton(app, &game->joinGameButton);
    Graphics_DrawButton(app, &game->backButton);
}
void Graphics_RenderIPInput(AppContext *app, GameState *game)
{
    Graphics_DrawAnimatedBackground(app);
    int titleBob = (int)(sin(SDL_GetTicks() / 300.0) * 8);
    Graphics_DrawTextCentered(app, app->fontLarge, "Join Game", 120 + titleBob, (SDL_Color){90, 30, 140, 255});
    SDL_Rect input_box_rect = {100, 300, WINDOW_WIDTH - 200, 50};
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(app->renderer, &input_box_rect);
    int textWidth = 0, textHeight = 0;
    if (strlen(game->targetIP) > 0)
    {
        TTF_SizeText(app->fontRegular, game->targetIP, &textWidth, &textHeight);
        Graphics_DrawText(app, app->fontRegular, game->targetIP, input_box_rect.x + 15, input_box_rect.y + 12, (SDL_Color){0, 0, 0, 255});
    }
    Graphics_DrawTextCentered(app, app->fontRegular, "Press ENTER to Connect", 400, (SDL_Color){50, 50, 50, 255});
    Graphics_DrawButton(app, &game->backButton);
}
void Graphics_RenderNameInput(AppContext *app, GameState *game)
{
    Graphics_DrawAnimatedBackground(app);
    int titleBob = (int)(sin(SDL_GetTicks() / 300.0) * 8);
    Graphics_DrawTextCentered(app, app->fontLarge, "Enter Player Names", 120 + titleBob, (SDL_Color){90, 30, 140, 255});
    char prompt[50];
    sprintf(prompt, "Enter Player %d Name:", game->currentNameInput + 1);
    Graphics_DrawTextCentered(app, app->fontRegular, prompt, 230, (SDL_Color){50, 50, 50, 255});
    SDL_Rect input_box_rect = {100, 300, WINDOW_WIDTH - 200, 50};
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(app->renderer, &input_box_rect);
    const char *currentName = game->playerNames[game->currentNameInput];
    int textWidth = 0, textHeight = 0;
    if (strlen(currentName) > 0)
    {
        TTF_SizeText(app->fontRegular, currentName, &textWidth, &textHeight);
        Graphics_DrawText(app, app->fontRegular, currentName, input_box_rect.x + 15, input_box_rect.y + 12, (SDL_Color){0, 0, 0, 255});
    }
    Graphics_DrawTextCentered(app, app->fontRegular, "Press ENTER to confirm", 400, (SDL_Color){50, 50, 50, 255});
}
void Graphics_RenderTutorial(AppContext *app, GameState *game)
{
    Graphics_DrawAnimatedBackground(app);
    Graphics_DrawTextCentered(app, app->fontLarge, "HOW TO PLAY", 150, (SDL_Color){90, 30, 140, 255});
    SDL_Color textColor = {50, 50, 50, 255};
    if (game->gameMode == 0)
    {
        Graphics_DrawTextCentered(app, app->fontLarge, "CLASSIC MODE", 250, (SDL_Color){200, 100, 50, 255});
        Graphics_DrawTextCentered(app, app->fontRegular, "Take turns placing letters to form words.", 330, textColor);
        Graphics_DrawTextCentered(app, app->fontRegular, "Build off your opponent's letters to score points.", 370, textColor);
    }
    else if (game->gameMode == 1)
    {
        Graphics_DrawTextCentered(app, app->fontLarge, "BLITZ MODE", 250, (SDL_Color){200, 50, 50, 255});
        Graphics_DrawTextCentered(app, app->fontRegular, "You only have 7 seconds per turn.", 330, textColor);
        Graphics_DrawTextCentered(app, app->fontRegular, "If you don't play fast, you lose your turn!", 370, textColor);
    }
    int timeLeft = 6 - ((SDL_GetTicks() - game->tutorialStartTime) / 1000);
    if (timeLeft < 1)
        timeLeft = 1;
    char timeText[100];
    sprintf(timeText, "Game starting in %d...", timeLeft);
    Graphics_DrawTextCentered(app, app->fontRegular, timeText, 550, (SDL_Color){150, 50, 50, 255});
    Graphics_DrawTextCentered(app, app->fontRegular, "(Click or Press Any Key to Skip)", 600, (SDL_Color){100, 100, 100, 255});
}

void Graphics_RenderGame(AppContext *app, GameState *game)
{
    SDL_SetRenderDrawColor(app->renderer, 230, 220, 245, 255);
    SDL_RenderClear(app->renderer);

    int startX = (WINDOW_WIDTH - (GRID_COLS * TILE_SIZE)) / 2;
    int startY = 45 + (WINDOW_HEIGHT - 60 - 45 - (GRID_ROWS * TILE_SIZE)) / 2;

    SDL_SetRenderDrawColor(app->renderer, 200, 180, 220, 255);
    for (int i = 0; i <= GRID_COLS; i++)
    {
        SDL_RenderDrawLine(app->renderer, startX + i * TILE_SIZE, startY, startX + i * TILE_SIZE, startY + GRID_ROWS * TILE_SIZE);
    }
    for (int i = 0; i <= GRID_ROWS; i++)
    {
        SDL_RenderDrawLine(app->renderer, startX, startY + i * TILE_SIZE, startX + GRID_COLS * TILE_SIZE, startY + i * TILE_SIZE);
    }

    for (int x = 0; x < GRID_COLS; x++)
    {
        for (int y = 0; y < GRID_ROWS; y++)
        {
            if (game->grid[x][y])
            {
                int px = startX + x * TILE_SIZE;
                int py = startY + y * TILE_SIZE;
                SDL_SetRenderDrawColor(app->renderer, 130, 110, 150, 255);
                SDL_Rect shadow = {px + 3, py + 3, TILE_SIZE - 4, TILE_SIZE - 4};
                SDL_RenderFillRect(app->renderer, &shadow);
                if (SDL_GetTicks() < game->highlightEndTime[x][y])
                    SDL_SetRenderDrawColor(app->renderer, 255, 204, 51, 255);
                else
                    SDL_SetRenderDrawColor(app->renderer, 255, 250, 240, 255);
                SDL_Rect tileBase = {px + 1, py + 1, TILE_SIZE - 4, TILE_SIZE - 4};
                SDL_RenderFillRect(app->renderer, &tileBase);
                char letter[2] = {game->grid[x][y], '\0'};
                int ltw = 0, lth = 0;
                TTF_SizeText(app->fontRegular, letter, &ltw, &lth);
                if (ltw == 0)
                    ltw = 14;
                if (lth == 0)
                    lth = 24;
                Graphics_DrawText(app, app->fontRegular, letter, px + (TILE_SIZE - ltw) / 2, py + (TILE_SIZE - lth) / 2, (SDL_Color){0, 0, 0, 255});
            }
        }
    }

    if (game->isTileSelected)
    {
        SDL_SetRenderDrawColor(app->renderer, 255, 0, 0, 255);
        SDL_Rect sRect = {startX + game->selectedX * TILE_SIZE, startY + game->selectedY * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        for (int b = 0; b < 4; b++)
        {
            SDL_Rect thickRect = {sRect.x + b, sRect.y + b, sRect.w - (b * 2), sRect.h - (b * 2)};
            SDL_RenderDrawRect(app->renderer, &thickRect);
        }
        if (game->currentLetter)
        {
            char letter[2] = {game->currentLetter, '\0'};
            int ltw = 0, lth = 0;
            TTF_SizeText(app->fontRegular, letter, &ltw, &lth);
            Graphics_DrawText(app, app->fontRegular, letter, sRect.x + (TILE_SIZE - ltw) / 2, sRect.y + (TILE_SIZE - lth) / 2, (SDL_Color){255, 0, 0, 255});
        }
    }

    SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(app->renderer, 50, 40, 70, 240);
    SDL_Rect topBar = {0, 0, WINDOW_WIDTH, 45};
    SDL_RenderFillRect(app->renderer, &topBar);
    SDL_Rect btmBar = {0, WINDOW_HEIGHT - 60, WINDOW_WIDTH, 60};
    SDL_RenderFillRect(app->renderer, &btmBar);
    SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_NONE);

    const char *p1Name = (strlen(game->playerNames[0]) > 0) ? game->playerNames[0] : "Player 1";
    const char *p2Name = (strlen(game->playerNames[1]) > 0) ? game->playerNames[1] : "Player 2";
    const char *turnName = (game->currentPlayer == 0) ? p1Name : p2Name;
    char scoreText[200];
    sprintf(scoreText, "%s: %d   |   %s: %d   |   TURN: %s", p1Name, game->scores[0], p2Name, game->scores[1], turnName);
    Graphics_DrawText(app, app->fontRegular, scoreText, 15, 10, (SDL_Color){255, 255, 255, 255});

    Uint32 elapsedTime = SDL_GetTicks() - game->gameStartTime;
    Uint32 timeLeft = (elapsedTime >= GAME_DURATION_MS) ? 0 : GAME_DURATION_MS - elapsedTime;
    int totalSeconds = timeLeft / 1000;
    char timerText[20];
    sprintf(timerText, "Time: %02d:%02d", totalSeconds / 60, totalSeconds % 60);
    int tw = 0, th = 0;
    TTF_SizeText(app->fontRegular, timerText, &tw, &th);
    Graphics_DrawText(app, app->fontRegular, timerText, WINDOW_WIDTH - tw - 20, 10, (SDL_Color){255, 255, 255, 255});

    if (!game->isMultiplayer || game->currentPlayer == game->localPlayerId)
    {
        Uint32 turnLimit = (game->gameMode == 1) ? BLITZ_TURN_DURATION_MS : TURN_DURATION_MS;
        Uint32 turnElapsed = SDL_GetTicks() - game->turnStartTime;
        float ratio = 1.0f - ((float)turnElapsed / turnLimit);
        if (ratio < 0)
            ratio = 0;
        SDL_Rect barBg = {10, WINDOW_HEIGHT - 35, 200, 12};
        SDL_Rect barFg = {10, WINDOW_HEIGHT - 35, (int)(200 * ratio), 12};
        SDL_SetRenderDrawColor(app->renderer, 100, 50, 50, 255);
        SDL_RenderFillRect(app->renderer, &barBg);
        SDL_SetRenderDrawColor(app->renderer, (int)((1.0 - ratio) * 255), (int)(ratio * 255), 50, 255);
        SDL_RenderFillRect(app->renderer, &barFg);
    }

    game->pauseButton.rect = (SDL_Rect){WINDOW_WIDTH - 250, WINDOW_HEIGHT - 50, 110, 40};
    game->giveUpButton.rect = (SDL_Rect){WINDOW_WIDTH - 130, WINDOW_HEIGHT - 50, 110, 40};
    game->pauseButton.text = game->isPaused ? "Resume" : "Pause";
    Graphics_DrawButton(app, &game->pauseButton);
    Graphics_DrawButton(app, &game->giveUpButton);

    if (game->isPaused)
    {
        SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 180);
        SDL_Rect full = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(app->renderer, &full);
        SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_NONE);
        Graphics_DrawTextCentered(app, app->fontLarge, "PAUSED", WINDOW_HEIGHT / 2 - 40, (SDL_Color){255, 255, 255, 255});
        Graphics_DrawTextCentered(app, app->fontRegular, "Press any key to resume", WINDOW_HEIGHT / 2 + 10, (SDL_Color){200, 200, 200, 255});
    }
}

void Graphics_RenderGameOver(AppContext *app, GameState *game)
{
    Graphics_DrawAnimatedBackground(app);
    Graphics_DrawTextCentered(app, app->fontLarge, "GAME OVER", 100, (SDL_Color){90, 30, 140, 255});

    if (strlen(game->endReason) > 0)
        Graphics_DrawTextCentered(app, app->fontRegular, game->endReason, 150, (SDL_Color){255, 255, 50, 255});

    const char *p1Name = (strlen(game->playerNames[0]) > 0) ? game->playerNames[0] : "Player 1";
    const char *p2Name = (strlen(game->playerNames[1]) > 0) ? game->playerNames[1] : "Player 2";
    char p1Score[100], p2Score[100];
    sprintf(p1Score, "%s's Score: %d", p1Name, game->scores[0]);
    sprintf(p2Score, "%s's Score: %d", p2Name, game->scores[1]);
    Graphics_DrawTextCentered(app, app->fontRegular, p1Score, 200, (SDL_Color){0, 0, 0, 255});
    Graphics_DrawTextCentered(app, app->fontRegular, p2Score, 240, (SDL_Color){0, 0, 0, 255});

    char stat1[150], stat2[150];
    if (game->longestWordLen > 0)
    {
        sprintf(stat1, "Longest Word: %s (%d letters)", game->longestWord, game->longestWordLen);
        sprintf(stat2, "Highest Scoring Word: %s (%d pts)", game->bestWord, game->bestWordScore);
    }
    else
    {
        sprintf(stat1, "- No words played -");
        sprintf(stat2, "");
    }
    Graphics_DrawTextCentered(app, app->fontRegular, stat1, 300, (SDL_Color){200, 100, 30, 255});
    Graphics_DrawTextCentered(app, app->fontRegular, stat2, 340, (SDL_Color){200, 100, 30, 255});

    if (!game->isSinglePlayer && game->forfeitedPlayer == -1 && (game->turnsPlayed[0] > 0 || game->turnsPlayed[1] > 0))
    {
        float p1Avg = game->turnsPlayed[0] == 0 ? 999.0f : (game->totalTurnTime[0] / 1000.0f) / game->turnsPlayed[0];
        float p2Avg = game->turnsPlayed[1] == 0 ? 999.0f : (game->totalTurnTime[1] / 1000.0f) / game->turnsPlayed[1];
        float p1Smart = (game->bonusWordsCount[0] * 10.0f) - p1Avg;
        float p2Smart = (game->bonusWordsCount[1] * 10.0f) - p2Avg;
        char smartText[150];
        if (p1Smart > p2Smart)
            sprintf(smartText, "Tactical Genius: %s (%.1fs avg)", p1Name, p1Avg);
        else if (p2Smart > p1Smart)
            sprintf(smartText, "Tactical Genius: %s (%.1fs avg)", p2Name, p2Avg);
        else
            sprintf(smartText, "Tactical Genius: Tie!");
        Graphics_DrawTextCentered(app, app->fontRegular, smartText, 400, (SDL_Color){20, 150, 200, 255});
    }

    int winner = -1;
    if (game->forfeitedPlayer == 0)
        winner = 1;
    else if (game->forfeitedPlayer == 1)
        winner = 0;
    else if (game->scores[0] > game->scores[1])
        winner = 0;
    else if (game->scores[1] > game->scores[0])
        winner = 1;
    else
        winner = 2;
    char winnerText[100];
    if (game->forfeitedPlayer != -1)
        sprintf(winnerText, "%s WINS by Forfeit!", winner == 0 ? p1Name : p2Name);
    else if (winner == 2)
        sprintf(winnerText, "IT'S A TIE!");
    else
        sprintf(winnerText, "%s WINS!", winner == 0 ? p1Name : p2Name);

    int pulse = (int)(sin(SDL_GetTicks() / 150.0) * 50) + 150;
    Graphics_DrawTextCentered(app, app->fontLarge, winnerText, 480, (SDL_Color){0, pulse, 0, 255});

    game->playAgainButton.rect = (SDL_Rect){(WINDOW_WIDTH - 200) / 2, 550, 200, 50};
    game->backButton.rect = (SDL_Rect){(WINDOW_WIDTH - 200) / 2, 620, 200, 50};
    Graphics_DrawButton(app, &game->playAgainButton);
    Graphics_DrawButton(app, &game->backButton);
}

static void DrawExampleWord(AppContext *app, int startX, int startY, const char *word, int highlightIndex, int highlightColorCode)
{
    int boxSize = 40;
    int gap = 5;
    SDL_Color whiteText = {255, 255, 255, 255};
    SDL_Color darkBorder = {50, 50, 80, 255};
    for (int i = 0; i < 5; i++)
    {
        SDL_Rect box = {startX + (i * (boxSize + gap)), startY, boxSize, boxSize};
        if (i == highlightIndex)
        {
            if (highlightColorCode == 3)
                SDL_SetRenderDrawColor(app->renderer, 83, 141, 78, 255);
            else if (highlightColorCode == 2)
                SDL_SetRenderDrawColor(app->renderer, 181, 159, 59, 255);
            else
                SDL_SetRenderDrawColor(app->renderer, 60, 60, 70, 255);
            SDL_RenderFillRect(app->renderer, &box);
            SDL_SetRenderDrawColor(app->renderer, 30, 30, 50, 100);
            SDL_RenderDrawRect(app->renderer, &box);
        }
        else
        {
            SDL_SetRenderDrawColor(app->renderer, 30, 30, 60, 150);
            SDL_RenderFillRect(app->renderer, &box);
            SDL_SetRenderDrawColor(app->renderer, darkBorder.r, darkBorder.g, darkBorder.b, darkBorder.a);
            SDL_RenderDrawRect(app->renderer, &box);
        }
        char letter[2] = {word[i], '\0'};
        int ltw = 0, lth = 0;
        TTF_SizeText(app->fontRegular, letter, &ltw, &lth);
        if (ltw == 0)
            ltw = 14;
        if (lth == 0)
            lth = 24;
        Graphics_DrawText(app, app->fontRegular, letter, box.x + (boxSize - ltw) / 2, box.y + (boxSize - lth) / 2, whiteText);
    }
}
void Graphics_RenderMysteryTutorial(AppContext *app, GameState *game)
{
    Graphics_DrawMysteryBackground(app);
    SDL_Color whiteText = {255, 255, 255, 255};
    SDL_Color lightBlueGray = {200, 200, 220, 255};
    Graphics_DrawText(app, app->fontLarge, "How To Play", 50, 50, whiteText);
    Graphics_DrawText(app, app->fontRegular, "Guess the Mystery Word in 6 tries.", 50, 110, lightBlueGray);
    Graphics_DrawText(app, app->fontRegular, "- Each guess must be a valid 5-letter word.", 50, 160, lightBlueGray);
    Graphics_DrawText(app, app->fontRegular, "- The color of the tiles will change to show", 50, 200, lightBlueGray);
    Graphics_DrawText(app, app->fontRegular, "  how close your guess was to the word.", 50, 230, lightBlueGray);
    SDL_SetRenderDrawColor(app->renderer, 60, 60, 90, 255);
    SDL_RenderDrawLine(app->renderer, 50, 280, 750, 280);
    Graphics_DrawText(app, app->fontRegular, "Examples", 50, 300, whiteText);
    DrawExampleWord(app, 50, 340, "WORDY", 0, 3);
    Graphics_DrawText(app, app->fontRegular, "W is in the word and in the correct spot.", 50, 390, lightBlueGray);
    DrawExampleWord(app, 50, 440, "LIGHT", 1, 2);
    Graphics_DrawText(app, app->fontRegular, "I is in the word but in the wrong spot.", 50, 490, lightBlueGray);
    DrawExampleWord(app, 50, 540, "ROGUE", 3, 1);
    Graphics_DrawText(app, app->fontRegular, "U is not in the word in any spot.", 50, 590, lightBlueGray);
    Graphics_DrawTextCentered(app, app->fontRegular, "Press any key or click to start...", 700, (SDL_Color){150, 150, 180, 255});
}
void Graphics_RenderMystery(AppContext *app, GameState *game)
{
    Graphics_DrawMysteryBackground(app);
    Graphics_DrawTextCentered(app, app->fontLarge, "Mystery Word", 30, (SDL_Color){255, 255, 255, 255});
    SDL_SetRenderDrawColor(app->renderer, 60, 60, 90, 255);
    SDL_RenderDrawLine(app->renderer, 0, 90, WINDOW_WIDTH, 90);
    int boxSize = 65;
    int gap = 8;
    int gridWidth = (boxSize * MYSTERY_COLS) + (gap * (MYSTERY_COLS - 1));
    int gridHeight = (boxSize * MYSTERY_ROWS) + (gap * (MYSTERY_ROWS - 1));
    int startX = (WINDOW_WIDTH - gridWidth) / 2;
    int startY = (WINDOW_HEIGHT - gridHeight) / 2 + 20;
    for (int r = 0; r < MYSTERY_ROWS; r++)
    {
        for (int c = 0; c < MYSTERY_COLS; c++)
        {
            SDL_Rect box = {startX + (c * (boxSize + gap)), startY + (r * (boxSize + gap)), boxSize, boxSize};
            int colorCode = game->mysteryColors[r][c];
            SDL_Color textColor = {255, 255, 255, 255};
            SDL_Color borderColor = {20, 20, 50, 255};
            if (colorCode == 0)
            {
                SDL_SetRenderDrawColor(app->renderer, 30, 30, 60, 150);
                SDL_RenderFillRect(app->renderer, &box);
                if (r == game->mysteryRow)
                    SDL_SetRenderDrawColor(app->renderer, 80, 80, 130, 255);
                else
                    SDL_SetRenderDrawColor(app->renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
                SDL_RenderDrawRect(app->renderer, &box);
            }
            else
            {
                if (colorCode == 1)
                    SDL_SetRenderDrawColor(app->renderer, 60, 60, 70, 255);
                else if (colorCode == 2)
                    SDL_SetRenderDrawColor(app->renderer, 181, 159, 59, 255);
                else if (colorCode == 3)
                    SDL_SetRenderDrawColor(app->renderer, 83, 141, 78, 255);
                SDL_RenderFillRect(app->renderer, &box);
                SDL_SetRenderDrawColor(app->renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
                SDL_RenderDrawRect(app->renderer, &box);
            }
            if (game->mysteryGrid[r][c] != '\0')
            {
                char letter[2] = {game->mysteryGrid[r][c], '\0'};
                int ltw = 0, lth = 0;
                TTF_SizeText(app->fontLarge, letter, &ltw, &lth);
                if (ltw == 0)
                    ltw = 20;
                if (lth == 0)
                    lth = 40;
                Graphics_DrawText(app, app->fontLarge, letter, box.x + (boxSize - ltw) / 2, box.y + (boxSize - lth) / 2 - 5, textColor);
            }
        }
    }
    Graphics_DrawTextCentered(app, app->fontRegular, "Type your guess. Press ENTER to Submit.", WINDOW_HEIGHT - 60, (SDL_Color){150, 150, 180, 255});
    game->backButton.rect = (SDL_Rect){20, 20, 100, 40};
    game->backButton.text = "Quit";
    Graphics_DrawButton(app, &game->backButton);
}
void Graphics_RenderMysteryGameOver(AppContext *app, GameState *game)
{
    Graphics_RenderMystery(app, game);
    SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(app->renderer, 10, 5, 20, 220);
    SDL_Rect fullScreen = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(app->renderer, &fullScreen);
    SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_NONE);
    if (game->mysteryState == 1)
        Graphics_DrawTextCentered(app, app->fontLarge, "SPLENDID!", 300, (SDL_Color){83, 141, 78, 255});
    else
    {
        char msg[100];
        sprintf(msg, "The word was: %s", game->mysteryTarget);
        Graphics_DrawTextCentered(app, app->fontLarge, msg, 300, (SDL_Color){200, 80, 80, 255});
    }
    game->playAgainButton.rect = (SDL_Rect){(WINDOW_WIDTH - 200) / 2, 400, 200, 50};
    game->backButton.rect = (SDL_Rect){(WINDOW_WIDTH - 200) / 2, 480, 200, 50};
    game->backButton.text = "Main Menu";
    Graphics_DrawButton(app, &game->playAgainButton);
    Graphics_DrawButton(app, &game->backButton);
}
void Graphics_RenderWaitingForOpponent(AppContext *app, GameState *game)
{
    Graphics_DrawAnimatedBackground(app);
    int titleBob = (int)(sin(SDL_GetTicks() / 300.0) * 8);
    Graphics_DrawTextCentered(app, app->fontLarge, "Waiting for Opponent...", 200 + titleBob, (SDL_Color){90, 30, 140, 255});
    char ipText[100];
    sprintf(ipText, "Your IP Address: %s", Network_GetLocalIP());
    Graphics_DrawTextCentered(app, app->fontRegular, ipText, 300 + titleBob, (SDL_Color){200, 50, 50, 255});
    Graphics_DrawTextCentered(app, app->fontRegular, "Give this IP to Player 2 so they can join!", 380 + titleBob, (SDL_Color){100, 100, 100, 255});
}