#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>

// --- THE NEW JAVA MATH RULES ---
#define NETWORK_PORT 9021
#define WIN_SCORE 400
#define MERCY_LEAD 70
#define MAX_BOARD_FILL 0.85

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// THE RECTANGULAR GRID
#define GRID_COLS 20
#define GRID_ROWS 10
#define TILE_SIZE 50

#define MYSTERY_ROWS 6
#define MYSTERY_COLS 5

#define GAME_DURATION_MS 300000
#define WARNING_TIME_MS 60000
#define WARNING_INTERVAL_MS 5000
#define TURN_DURATION_MS 20000
#define BLITZ_TURN_DURATION_MS 7000
#define MAX_WORD_LENGTH 25
#define MAX_SCORED_WORDS 100
#define BONUS_WORD_POINTS 50
#define MAX_NAME_LENGTH 16

typedef enum {
    STATE_SPLASH, STATE_MODE_MENU, STATE_OPPONENT_MENU, STATE_BOT_MENU, STATE_MULTIPLAYER_MENU,
    STATE_ENTER_IP, STATE_GET_NAMES, STATE_TUTORIAL, STATE_WAITING_FOR_OPPONENT,
    STATE_PLAYING, STATE_GAME_OVER, STATE_RESTART, STATE_QUIT,
    STATE_MYSTERY_TUTORIAL, STATE_MYSTERY_PLAYING, STATE_MYSTERY_GAMEOVER
} GameStateEnum;

typedef struct { SDL_Rect rect; const char* text; bool isHovered; } Button;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *fontRegular;
    TTF_Font *fontLarge;
    Mix_Chunk *sfxPlaceTile;
    Mix_Chunk *sfxValidWord;
    Mix_Chunk *sfxWarning;
    Mix_Chunk *sfxWin;
    Mix_Music *bgmWelcome;
} AppContext;

typedef struct {
    GameStateEnum currentState;
    int currentNameInput;
    char playerNames[2][MAX_NAME_LENGTH];
    char targetIP[16];
    bool isSinglePlayer;
    int botDifficulty;
    bool isMultiplayer;
    int localPlayerId;
    bool playerReady[2];

    char grid[GRID_COLS][GRID_ROWS];
    long highlightEndTime[GRID_COLS][GRID_ROWS];
    
    int currentPlayer;
    char currentLetter;
    bool isTileSelected;
    int selectedX, selectedY;
    int scores[2];
    char scoredWords[MAX_SCORED_WORDS][MAX_WORD_LENGTH];
    int scoredWordCount;

    Uint32 gameStartTime;
    Uint32 lastWarningTime;
    Uint32 turnStartTime;
    Uint32 botThinkStartTime;
    Uint32 tutorialStartTime;
    Uint32 lastFrameTime;

    bool isPaused;
    int gameMode;
    char longestWord[MAX_WORD_LENGTH];
    int longestWordLen;
    char bestWord[MAX_WORD_LENGTH];
    int bestWordScore;

    // ANALYTICS & WIN TRACKERS
    int forfeitedPlayer;
    Uint32 totalTurnTime[2];
    int turnsPlayed[2];
    int bonusWordsCount[2];
    char endReason[100]; 

    char mysteryTarget[6];
    char mysteryGrid[MYSTERY_ROWS][MYSTERY_COLS];
    int mysteryColors[MYSTERY_ROWS][MYSTERY_COLS];
    int mysteryRow;
    int mysteryCol;
    int mysteryState;

    Button playWordsCollideButton, playMysteryButton, classicModeButton, blitzModeButton;
    Button singlePlayerButton, startGameButton, multiplayerButton, botEasyButton, botHardButton;
    Button hostGameButton, joinGameButton, backButton, giveUpButton, pauseButton, playAgainButton;
} GameState;

#endif