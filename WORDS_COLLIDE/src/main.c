#include <stdio.h>
#include <string.h>
#include "../include/types.h"
#include "../include/logic.h"
#include "../include/audio.h"
#include "../include/graphics.h"
#include "../include/input.h"
#include "../include/network.h"

static bool Core_InitSDL(AppContext *app) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) return false;
    if (TTF_Init() == -1) return false;

    app->window = SDL_CreateWindow("WORDS_COLLIDE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
    if (!app->window) return false;
    
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_SOFTWARE);
    if (!app->renderer) return false;
    
    app->fontRegular = TTF_OpenFont("assets/arial.ttf", 24);
    app->fontLarge = TTF_OpenFont("assets/arial.ttf", 48);
    
    Network_Init();

    return Audio_Init(app);
}

static void Core_InitGame(GameState *game, AppContext *app) {
    game->currentState = STATE_SPLASH;
    game->currentNameInput = 0;
    game->gameStartTime = 0; 
    game->lastWarningTime = 0; 
    game->turnStartTime = 0;
    memset(game->playerNames[0], 0, MAX_NAME_LENGTH);
    memset(game->playerNames[1], 0, MAX_NAME_LENGTH);
    memset(game->targetIP, 0, sizeof(game->targetIP));
    memset(game->grid, 0, sizeof(game->grid));
    game->currentPlayer = 0;
    game->currentLetter = '\0';
    game->isTileSelected = false;
    game->selectedX = -1;
    game->selectedY = -1;
    game->scores[0] = 0;
    game->scores[1] = 0;
    game->scoredWordCount = 0;
    memset(game->scoredWords, 0, sizeof(game->scoredWords));

    game->startGameButton.rect = (SDL_Rect){ (WINDOW_WIDTH - 250) / 2, 350, 250, 50 };
    game->startGameButton.text = "Play on One Device";
    game->startGameButton.isHovered = false;

    game->multiplayerButton.rect = (SDL_Rect){ (WINDOW_WIDTH - 250) / 2, 430, 250, 50 };
    game->multiplayerButton.text = "Multiplayer (Wi-Fi)";
    game->multiplayerButton.isHovered = false;

    game->hostGameButton.rect = (SDL_Rect){ (WINDOW_WIDTH - 250) / 2, 330, 250, 50 };
    game->hostGameButton.text = "Host a Game";
    game->hostGameButton.isHovered = false;

    game->joinGameButton.rect = (SDL_Rect){ (WINDOW_WIDTH - 250) / 2, 410, 250, 50 };
    game->joinGameButton.text = "Join a Game";
    game->joinGameButton.isHovered = false;

    game->backButton.rect = (SDL_Rect){ (WINDOW_WIDTH - 250) / 2, 490, 250, 50 };
    game->backButton.text = "Back";
    game->backButton.isHovered = false;

    game->giveUpButton.rect = (SDL_Rect){ WINDOW_WIDTH - 130, WINDOW_HEIGHT - 45, 120, 35 };
    game->giveUpButton.text = "Give Up";
    game->giveUpButton.isHovered = false;

    game->playAgainButton.rect = (SDL_Rect){ (WINDOW_WIDTH - 150) / 2, 480, 150, 40 };
    game->playAgainButton.text = "Play Again";
    game->playAgainButton.isHovered = false;

    Audio_PlayMusic(app->bgmWelcome);
}

static void Core_Cleanup(AppContext *app) {
    SDL_StopTextInput();
    Network_Cleanup(); 
    Audio_Cleanup(app);
    if (app->fontRegular) TTF_CloseFont(app->fontRegular);
    if (app->fontLarge) TTF_CloseFont(app->fontLarge); 
    if (app->renderer) SDL_DestroyRenderer(app->renderer);
    if (app->window) SDL_DestroyWindow(app->window);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    AppContext app = {0}; 
    GameState game;

    Logic_LoadDictionary();
    if (!Core_InitSDL(&app)) return 1;

    Core_InitGame(&game, &app); 
    SDL_Event event;
    
    while (game.currentState != STATE_QUIT) {
        if (game.currentState == STATE_RESTART) {
            Core_InitGame(&game, &app); 
            continue; 
        }
        
        if (game.currentState == STATE_PLAYING) {
            Uint32 currentTime = SDL_GetTicks();
            Uint32 elapsedMs = currentTime - game.gameStartTime;
            
            if (elapsedMs >= GAME_DURATION_MS) {
                Audio_StopMusic(); 
                Audio_PlaySound(app.sfxWin); 
                game.currentState = STATE_GAME_OVER;
                SDL_StopTextInput(); 
            }
            
            Uint32 remainingMs = GAME_DURATION_MS - elapsedMs;
            if (remainingMs < WARNING_TIME_MS) {
                if (currentTime - game.lastWarningTime > WARNING_INTERVAL_MS) {
                    Audio_PlaySound(app.sfxWarning); 
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
            if (Network_ReceiveMove(&enemyMove)) {
                Audio_PlaySound(app.sfxPlaceTile);
                game.grid[enemyMove.x][enemyMove.y] = enemyMove.letter;
                
                int score = Logic_CheckAndScore(&game, enemyMove.x, enemyMove.y, enemyMove.letter);
                if (score > 0) {
                    Audio_PlaySound(app.sfxValidWord);
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
                case STATE_SPLASH:             Input_HandleSplash(&event, &game); break;
                case STATE_MULTIPLAYER_MENU:   Input_HandleMultiplayerMenu(&event, &game); break;
                case STATE_ENTER_IP:           Input_HandleIPInput(&event, &game); break;
                case STATE_GET_NAMES:          Input_HandleNames(&event, &game, &app); break;
                case STATE_PLAYING:            Input_HandleGame(&event, &game, &app); break;
                case STATE_GAME_OVER:          Input_HandleGameOver(&event, &game); break;
                default: break; 
            }
        }

        SDL_RaiseWindow(app.window);
        
        switch (game.currentState) {
            case STATE_SPLASH:             Graphics_RenderSplashScreen(&app, &game); break;
            case STATE_MULTIPLAYER_MENU:   Graphics_RenderMultiplayerMenu(&app, &game); break;
            case STATE_ENTER_IP:           Graphics_RenderIPInput(&app, &game); break;
            case STATE_GET_NAMES:          Graphics_RenderNameInput(&app, &game); break;
            case STATE_PLAYING:            Graphics_RenderGame(&app, &game); break;
            case STATE_GAME_OVER:          Graphics_RenderGameOver(&app, &game); break;
            default: break;
        }
        
        SDL_Delay(10);
    }
    
    Core_Cleanup(&app);
    return 0;
}