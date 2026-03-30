#ifndef TYPES_HPP
#define TYPES_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <array>
#include <string>
#include "Config.hpp"

enum class GameStateType {
    SPLASH,
    MULTIPLAYER_MENU,
    ENTER_IP,
    GET_NAMES,
    PLAYING,
    GAME_OVER,
    RESTART,
    QUIT
};

struct Button {
    SDL_Rect rect{};
    std::string text;
    bool isHovered = false;
};

struct GameState {
    GameStateType currentState = GameStateType::SPLASH;
    std::array<std::string, 2> playerNames{};
    int currentNameInput = 0;
    std::string targetIP;
    Uint32 gameStartTime = 0;
    Uint32 lastWarningTime = 0;
    Uint32 turnStartTime = 0;

    std::array<std::array<char, GRID_SIZE>, GRID_SIZE> grid{};
    int currentPlayer = 0;
    char currentLetter = '\0';
    int selectedX = -1, selectedY = -1;
    bool isTileSelected = false;
    std::array<int, 2> scores{0, 0};
    std::array<std::string, MAX_SCORED_WORDS> scoredWords{};
    int scoredWordCount = 0;

    Button startGameButton;
    Button multiplayerButton;
    Button hostGameButton;
    Button joinGameButton;
    Button backButton;
    Button giveUpButton;
    Button playAgainButton;
};

struct AppContext {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* fontRegular = nullptr;
    TTF_Font* fontLarge = nullptr;
    Mix_Chunk* sfxPlaceTile = nullptr;
    Mix_Chunk* sfxValidWord = nullptr;
    Mix_Chunk* sfxWarning = nullptr;
    Mix_Chunk* sfxWin = nullptr;
    Mix_Music* bgmWelcome = nullptr;
};

#endif // TYPES_HPP
