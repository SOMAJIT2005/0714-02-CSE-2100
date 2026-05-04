#include "../include/logic.h"
#include "../include/network.h"
#include "../include/audio.h"
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

static char dictionary[MAX_WORDS_IN_DICT][32];
static int dictSize = 0;

void Logic_LoadDictionary(void) {
    FILE *file = fopen("assets/Dictionary.txt", "r");
    if (!file) {
        printf("WARNING: Could not load assets/Dictionary.txt\n");
        return;
    }
    char line[64];
    while (fgets(line, sizeof(line), file) && dictSize < MAX_WORDS_IN_DICT) {
        int len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
            len--;
        }
        for (int i = 0; i < len; i++) line[i] = toupper(line[i]);
        if (len > 0) {
            strcpy(dictionary[dictSize], line);
            dictSize++;
        }
    }
    fclose(file);
    printf("Dictionary loaded: %d words.\n", dictSize);
}

bool Logic_IsValidWord(const char *word) {
    for(int i=0; i < dictSize; i++) {
        if(strcmp(dictionary[i], word) == 0) return true;
    }
    return false;
}

void Logic_InitGameState(GameState *game) {
    memset(game, 0, sizeof(GameState));
    game->currentState = STATE_SPLASH;

    int midX = WINDOW_WIDTH / 2; // Usually 500
    
    // Splash
    game->playWordsCollideButton.rect = (SDL_Rect){midX - 175, 300, 350, 60};
    game->playWordsCollideButton.text = "Play WORDS COLLIDE";
    game->playMysteryButton.rect = (SDL_Rect){midX - 175, 380, 350, 60};
    game->playMysteryButton.text = "Play MYSTERY WORD";

    // Modes
    game->classicModeButton.rect = (SDL_Rect){midX - 150, 300, 300, 50};
    game->classicModeButton.text = "CLASSIC MODE";
    game->blitzModeButton.rect = (SDL_Rect){midX - 150, 380, 300, 50};
    game->blitzModeButton.text = "BLITZ MODE (7s)";

    // Opponent
    game->singlePlayerButton.rect = (SDL_Rect){midX - 150, 250, 300, 50};
    game->singlePlayerButton.text = "Play vs Bot";
    game->startGameButton.rect = (SDL_Rect){midX - 150, 320, 300, 50};
    game->startGameButton.text = "Local Co-Op";
    game->multiplayerButton.rect = (SDL_Rect){midX - 150, 390, 300, 50};
    game->multiplayerButton.text = "Multiplayer (Wi-Fi)";

    // Bot Options
    game->botEasyButton.rect = (SDL_Rect){midX - 150, 300, 300, 50};
    game->botEasyButton.text = "Easy: Scribble Sprite";
    game->botHardButton.rect = (SDL_Rect){midX - 150, 380, 300, 50};
    game->botHardButton.text = "Hard: Lexicon Leviathan";

    // Multiplayer Options
    game->hostGameButton.rect = (SDL_Rect){midX - 150, 300, 300, 50};
    game->hostGameButton.text = "Host Game";
    game->joinGameButton.rect = (SDL_Rect){midX - 150, 380, 300, 50};
    game->joinGameButton.text = "Join Game";

    // Back Button (At the bottom of the screen)
    game->backButton.rect = (SDL_Rect){midX - 100, 500, 200, 50};
    game->backButton.text = "Back";
    
    // In-Game Buttons
    game->pauseButton.rect = (SDL_Rect){WINDOW_WIDTH - 250, WINDOW_HEIGHT - 50, 110, 40};
    game->pauseButton.text = "Pause";
    game->giveUpButton.rect = (SDL_Rect){WINDOW_WIDTH - 130, WINDOW_HEIGHT - 50, 110, 40};
    game->giveUpButton.text = "Give Up";
}

void Logic_CheckWinConditions(GameState *game, AppContext *app) {
    if (game->currentState != STATE_PLAYING) return;
    bool shouldEnd = false;

    // RULE 1: 400 Point Limit
    if (game->scores[0] >= 400 || game->scores[1] >= 400) {
        strcpy(game->endReason, "Score Limit Reached (400+)!");
        shouldEnd = true;
    }
    // RULE 2: 70 Point Mercy Lead
    else if (abs(game->scores[0] - game->scores[1]) >= 70 && (game->scores[0] > 0 || game->scores[1] > 0)) {
        strcpy(game->endReason, "Dominance! (70+ Point Lead)");
        shouldEnd = true;
    }
    else {
        // RULE 3: 85% Board Fill
        int filled = 0;
        for (int x = 0; x < GRID_COLS; x++) {
            for (int y = 0; y < GRID_ROWS; y++) {
                if (game->grid[x][y] != '\0') filled++;
            }
        }
        if (filled >= (GRID_COLS * GRID_ROWS * 0.85f)) {
            strcpy(game->endReason, "Board is 85% Full!");
            shouldEnd = true;
        }
    }

    if (shouldEnd) {
        game->currentState = STATE_GAME_OVER;
        Audio_StopMusic();
        Audio_PlaySound(app->sfxWin);
    }
}

int Logic_CheckAndScore(GameState *game, int x, int y, char letter) {
    int score = 10; // Base valid placement score
    if (score > game->bestWordScore) {
        game->bestWordScore = score;
        sprintf(game->bestWord, "%c-WORD", letter);
    }
    game->longestWordLen = 5;
    strcpy(game->longestWord, "HELLO");
    return score;
}

void Logic_BotMove(GameState *game, AppContext *app) {
    for (int x = 0; x < GRID_COLS; x++) {
        for (int y = 0; y < GRID_ROWS; y++) {
            if (game->grid[x][y] == '\0') {
                game->grid[x][y] = 'A' + (rand() % 26);
                game->scores[1] += 10;
                game->turnsPlayed[1]++;
                game->currentPlayer = 0;
                game->turnStartTime = SDL_GetTicks();
                Audio_PlaySound(app->sfxPlaceTile);
                Logic_CheckWinConditions(game, app);
                return;
            }
        }
    }
}

void Logic_SetupMystery(GameState *game) {
    memset(game->mysteryGrid, '\0', sizeof(game->mysteryGrid));
    memset(game->mysteryColors, 0, sizeof(game->mysteryColors));
    game->mysteryRow = 0;
    game->mysteryCol = 0;
    game->mysteryState = 0;
    strcpy(game->mysteryTarget, "LOGIC");
}

bool Logic_CheckMysteryRow(GameState *game) {
    int correct = 0;
    for (int i = 0; i < 5; i++) {
        if (game->mysteryGrid[game->mysteryRow][i] == game->mysteryTarget[i]) {
            game->mysteryColors[game->mysteryRow][i] = 3;
            correct++;
        } else {
            game->mysteryColors[game->mysteryRow][i] = 1;
        }
    }
    if (correct == 5) game->mysteryState = 1;
    else {
        game->mysteryRow++;
        if (game->mysteryRow >= 6) game->mysteryState = 2;
    }
    return true;
}

void Logic_HandleNetworkMessage(GameState *game, AppContext *app, char *msg) {
    if (strncmp(msg, "READY", 5) == 0 || strncmp(msg, "READY_ACK", 9) == 0) {
        int pIndex; char name[16];
        sscanf(msg, "%*[^,],%d,%s", &pIndex, name);
        strcpy(game->playerNames[pIndex], name);
        game->playerReady[pIndex] = true;
        
        if (strncmp(msg, "READY", 5) == 0 && game->playerReady[game->localPlayerId]) {
            char buf[64];
            sprintf(buf, "READY_ACK,%d,%s\n", game->localPlayerId, game->playerNames[game->localPlayerId]);
            Network_SendMessage(buf);
        }
        if (game->playerReady[0] && game->playerReady[1]) {
            memset(game->grid, '\0', sizeof(game->grid));
            game->gameStartTime = SDL_GetTicks();
            game->turnStartTime = SDL_GetTicks();
            game->currentState = STATE_PLAYING;
        }
    }
    else if (strncmp(msg, "MOVE", 4) == 0) {
        int x, y; char letter;
        sscanf(msg, "MOVE,%d,%d,%c", &x, &y, &letter);
        game->grid[x][y] = letter;
        int score = Logic_CheckAndScore(game, x, y, letter);
        if (score > 0) game->scores[game->currentPlayer] += score;
        game->currentPlayer = (game->currentPlayer + 1) % 2;
        game->turnStartTime = SDL_GetTicks();
        Logic_CheckWinConditions(game, app);
    }
    else if (strncmp(msg, "PAUSE_ON", 8) == 0) game->isPaused = true;
    else if (strncmp(msg, "PAUSE_OFF", 9) == 0) game->isPaused = false;
    else if (strncmp(msg, "GIVE_UP", 7) == 0) {
        int pId; sscanf(msg, "GIVE_UP,%d", &pId);
        game->forfeitedPlayer = pId;
        strcpy(game->endReason, "Opponent Resigned");
        game->currentState = STATE_GAME_OVER;
    }
}