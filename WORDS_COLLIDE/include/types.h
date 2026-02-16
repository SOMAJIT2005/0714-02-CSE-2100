#ifndef TYPES_H
#define TYPES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include "config.h"

typedef enum {
    STATE_SPLASH,
    STATE_GET_NAMES,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_RESTART,
    STATE_QUIT
} GameStateType;

typedef struct {
    SDL_Rect rect;
    const char *text;
    bool isHovered;
} Button;

// Core game data
typedef struct {
    GameStateType currentState;
    char playerNames[2][MAX_NAME_LENGTH];
    int currentNameInput; 
    Uint32 gameStartTime; 
    Uint32 lastWarningTime; 
    Uint32 turnStartTime; 
    
    char grid[GRID_SIZE][GRID_SIZE];
    int currentPlayer;
    char currentLetter;
    int selectedX, selectedY;
    bool isTileSelected;
    int scores[2];
    char scoredWords[MAX_SCORED_WORDS][MAX_WORD_LENGTH];
    int scoredWordCount;

    Button giveUpButton;
    Button playAgainButton;
    Button startGameButton;
} GameState;

// Replaces all global variables (Dependency Injection)
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

#endif // TYPES_H