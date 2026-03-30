#include <iostream>
#include <cstring>
#include "Types.hpp"
#include "Audio.hpp"
#include "Graphics.hpp"
#include "Input.hpp"
#include "Logic.hpp"
#include "Network.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

static bool Core_InitSDL(AppContext& app) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) return false;
    if (TTF_Init() == -1) return false;

    app.window = SDL_CreateWindow("WORDS_COLLIDE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
    if (!app.window) return false;

    app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_SOFTWARE);
    if (!app.renderer) return false;

    app.fontRegular = TTF_OpenFont("assets/arial.ttf", 24);
    app.fontLarge = TTF_OpenFont("assets/arial.ttf", 48);

    Network::init();

    return true;
}

static void Core_InitGame(GameState& game, AppContext& app) {
    game.currentState = GameStateType::SPLASH;
    game.currentNameInput = 0;
    game.gameStartTime = 0;
    game.lastWarningTime = 0;
    game.turnStartTime = 0;
    game.playerNames = { std::string(), std::string() };
    game.targetIP.clear();
    for (auto &row : game.grid) row.fill(0);
    game.currentPlayer = 0;
    game.currentLetter = '\0';
    game.isTileSelected = false;
    game.selectedX = -1;
    game.selectedY = -1;
    game.scores = {0,0};
    game.scoredWordCount = 0;
    for (auto &s : game.scoredWords) s.clear();

    game.startGameButton.rect = { (WINDOW_WIDTH - 250) / 2, 350, 250, 50 };
    game.startGameButton.text = "Play on One Device";
    game.multiplayerButton.rect = { (WINDOW_WIDTH - 250) / 2, 430, 250, 50 };
    game.multiplayerButton.text = "Multiplayer (Wi-Fi)";
    game.hostGameButton.rect = { (WINDOW_WIDTH - 250) / 2, 330, 250, 50 };
    game.hostGameButton.text = "Host a Game";
    game.joinGameButton.rect = { (WINDOW_WIDTH - 250) / 2, 410, 250, 50 };
    game.joinGameButton.text = "Join a Game";
    game.backButton.rect = { (WINDOW_WIDTH - 250) / 2, 490, 250, 50 };
    game.backButton.text = "Back";
    game.giveUpButton.rect = { WINDOW_WIDTH - 130, WINDOW_HEIGHT - 45, 120, 35 };
    game.giveUpButton.text = "Give Up";
    game.playAgainButton.rect = { (WINDOW_WIDTH - 150) / 2, 480, 150, 40 };
    game.playAgainButton.text = "Play Again";
}

static void Core_Cleanup(AppContext& app) {
    SDL_StopTextInput();
    Network::cleanup();
    if (app.fontRegular) TTF_CloseFont(app.fontRegular);
    if (app.fontLarge) TTF_CloseFont(app.fontLarge);
    if (app.renderer) SDL_DestroyRenderer(app.renderer);
    if (app.window) SDL_DestroyWindow(app.window);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    AppContext app{};
    GameState game{};
    Audio audio(app);
    Graphics graphics(app);

    // Load dictionary first (keeps same order as original)
    Logic::loadDictionary();

    if (!Core_InitSDL(app)) {
        std::cerr << "Failed to init SDL\n";
        return 1;
    }

    if (!audio.init()) {
        std::cerr << "Failed to init audio\n";
        // continue without audio if desired
    }

    Core_InitGame(game, app);
    audio.playMusic(app.bgmWelcome);

    SDL_Event event;
    while (game.currentState != GameStateType::QUIT) {
        if (game.currentState == GameStateType::RESTART) {
            Core_InitGame(game, app);
            audio.playMusic(app.bgmWelcome);
            continue;
        }

        if (game.currentState == GameStateType::PLAYING) {
            Uint32 currentTime = SDL_GetTicks();
            Uint32 elapsedMs = currentTime - game.gameStartTime;

            if (elapsedMs >= GAME_DURATION_MS) {
                audio.stopMusic();
                audio.playSound(app.sfxWin);
                game.currentState = GameStateType::GAME_OVER;
                SDL_StopTextInput();
            }

            Uint32 remainingMs = GAME_DURATION_MS > elapsedMs ? GAME_DURATION_MS - elapsedMs : 0;
            if (remainingMs < WARNING_TIME_MS) {
                if (currentTime - game.lastWarningTime > WARNING_INTERVAL_MS) {
                    audio.playSound(app.sfxWarning);
                    game.lastWarningTime = currentTime;
                }
            }

            if (currentTime - game.turnStartTime >= TURN_DURATION_MS) {
                game.currentPlayer = (game.currentPlayer + 1) % 2;
                game.isTileSelected = false;
                game.currentLetter = '\0';
                game.turnStartTime = SDL_GetTicks();
                SDL_StopTextInput();
            }

            NetworkMove enemyMove;
            if (Network::receiveMove(enemyMove)) {
                audio.playSound(app.sfxPlaceTile);
                game.grid[enemyMove.x][enemyMove.y] = enemyMove.letter;
                int score = Logic::checkAndScore(game, enemyMove.x, enemyMove.y, enemyMove.letter);
                if (score > 0) {
                    audio.playSound(app.sfxValidWord);
                    game.scores[game.currentPlayer] += score;
                }
                game.currentPlayer = (game.currentPlayer + 1) % 2;
                game.isTileSelected = false;
                game.currentLetter = '\0';
                game.turnStartTime = SDL_GetTicks();
                SDL_StopTextInput();
            }
        }

        while (SDL_PollEvent(&event)) {
            switch (game.currentState) {
                case GameStateType::SPLASH: Input::handleSplash(event, game); break;
                case GameStateType::MULTIPLAYER_MENU: Input::handleMultiplayerMenu(event, game); break;
                case GameStateType::ENTER_IP: Input::handleIPInput(event, game); break;
                case GameStateType::GET_NAMES: Input::handleNames(event, game, app); break;
                case GameStateType::PLAYING: Input::handleGame(event, game, app); break;
                case GameStateType::GAME_OVER: Input::handleGameOver(event, game); break;
                default: break;
            }
        }

        SDL_RaiseWindow(app.window);

        switch (game.currentState) {
            case GameStateType::SPLASH: graphics.renderSplash(game); break;
            case GameStateType::MULTIPLAYER_MENU: graphics.renderMultiplayerMenu(game); break;
            case GameStateType::ENTER_IP: graphics.renderIPInput(game); break;
            case GameStateType::GET_NAMES: graphics.renderNameInput(game); break;
            case GameStateType::PLAYING: graphics.renderGame(game); break;
            case GameStateType::GAME_OVER: graphics.renderGameOver(game); break;
            default: break;
        }

        SDL_Delay(10);
    }

    Core_Cleanup(app);
    return 0;
}
