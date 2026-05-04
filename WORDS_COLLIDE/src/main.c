#include "../include/graphics.h"
#include "../include/input.h"
#include "../include/logic.h"
#include "../include/audio.h"
#include "../include/network.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    AppContext app = {0};
    GameState game = {0};

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) return -1;
    if (TTF_Init() < 0) return -1;
    Network_Init();
    Audio_Init(&app); 

    app.window = SDL_CreateWindow("Words Collide", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // FIXED: Properly loading from assets folder
    app.fontLarge = TTF_OpenFont("assets/arial.ttf", 52);
    app.fontRegular = TTF_OpenFont("assets/arial.ttf", 24);

    Logic_InitGameState(&game);
    Logic_LoadDictionary();

    bool running = true;
    SDL_Event event;
    char netBuffer[1024];

    while (running && game.currentState != STATE_QUIT) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) { running = false; break; }
            
            switch (game.currentState) {
                case STATE_SPLASH: Input_HandleSplash(&event, &game); break;
                case STATE_MODE_MENU: Input_HandleModeMenu(&event, &game); break;
                case STATE_OPPONENT_MENU: Input_HandleOpponentMenu(&event, &game); break;
                case STATE_BOT_MENU: Input_HandleBotMenu(&event, &game); break;
                case STATE_MULTIPLAYER_MENU: Input_HandleMultiplayerMenu(&event, &game); break;
                case STATE_ENTER_IP: Input_HandleIPInput(&event, &game); break;
                case STATE_GET_NAMES: Input_HandleNames(&event, &game, &app); break;
                case STATE_TUTORIAL: Input_HandleTutorial(&event, &game); break;
                case STATE_PLAYING: Input_HandleGame(&event, &game, &app); break;
                case STATE_GAME_OVER: Input_HandleGameOver(&event, &game); break;
                case STATE_MYSTERY_TUTORIAL: Input_HandleMysteryTutorial(&event, &game); break;
                case STATE_MYSTERY_PLAYING: Input_HandleMysteryPlaying(&event, &game, &app); break;
                case STATE_MYSTERY_GAMEOVER: Input_HandleMysteryGameOver(&event, &game); break;
                case STATE_RESTART: Logic_InitGameState(&game); break;
                default: break;
            }
        }

        // Bot Timer logic
        if (game.isSinglePlayer && game.currentState == STATE_PLAYING && game.currentPlayer == 1) {
            if (SDL_GetTicks() - game.turnStartTime > 1500) {
                Logic_BotMove(&game, &app);
            }
        }

        // Network processing
        if (game.isMultiplayer) {
            if (Network_ReceiveMessage(netBuffer, sizeof(netBuffer)) > 0) {
                Logic_HandleNetworkMessage(&game, &app, netBuffer);
            }
            if (game.currentState == STATE_WAITING_FOR_OPPONENT && game.localPlayerId == 0) {
                Network_BroadcastPresence(game.playerNames[0]);
            }
        }

        // Render Frame
        SDL_RenderClear(app.renderer);
        switch (game.currentState) {
            case STATE_SPLASH: Graphics_RenderSplashScreen(&app, &game); break;
            case STATE_MODE_MENU: Graphics_RenderModeMenu(&app, &game); break;
            case STATE_OPPONENT_MENU: Graphics_RenderOpponentMenu(&app, &game); break;
            case STATE_BOT_MENU: Graphics_RenderBotMenu(&app, &game); break;
            case STATE_MULTIPLAYER_MENU: Graphics_RenderMultiplayerMenu(&app, &game); break;
            case STATE_ENTER_IP: Graphics_RenderIPInput(&app, &game); break;
            case STATE_GET_NAMES: Graphics_RenderNameInput(&app, &game); break;
            case STATE_TUTORIAL: Graphics_RenderTutorial(&app, &game); break;
            case STATE_WAITING_FOR_OPPONENT: Graphics_RenderWaitingForOpponent(&app, &game); break;
            case STATE_PLAYING: Graphics_RenderGame(&app, &game); break;
            case STATE_GAME_OVER: Graphics_RenderGameOver(&app, &game); break;
            case STATE_MYSTERY_TUTORIAL: Graphics_RenderMysteryTutorial(&app, &game); break;
            case STATE_MYSTERY_PLAYING: Graphics_RenderMystery(&app, &game); break;
            case STATE_MYSTERY_GAMEOVER: Graphics_RenderMysteryGameOver(&app, &game); break;
            default: break;
        }
        SDL_RenderPresent(app.renderer);
        SDL_Delay(16);
    }

    Network_Cleanup();
    Audio_Cleanup(&app);
    if (app.fontLarge) TTF_CloseFont(app.fontLarge);
    if (app.fontRegular) TTF_CloseFont(app.fontRegular);
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}