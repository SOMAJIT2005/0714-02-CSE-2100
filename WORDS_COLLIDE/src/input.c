#include "../include/input.h"
#include "../include/audio.h"
#include "../include/logic.h"
#include <string.h>
#include <ctype.h>

static bool Input_IsMouseOverButton(SDL_Event *event, Button *button) {
    if (event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONDOWN) {
        int x = event->motion.x;
        int y = event->motion.y;
        return (x >= button->rect.x && x <= button->rect.x + button->rect.w &&
                y >= button->rect.y && y <= button->rect.y + button->rect.h);
    }
    return false;
}

void Input_HandleSplash(SDL_Event *event, GameState *game) {
    if (event->type == SDL_QUIT) game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION) {
        game->startGameButton.isHovered = Input_IsMouseOverButton(event, &game->startGameButton);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN && Input_IsMouseOverButton(event, &game->startGameButton)) {
        game->currentState = STATE_GET_NAMES;
        SDL_StartTextInput(); 
    }
}

void Input_HandleNames(SDL_Event *event, GameState *game, AppContext *app) {
    if (event->type == SDL_QUIT) game->currentState = STATE_QUIT;
    else if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_BACKSPACE) {
            int len = strlen(game->playerNames[game->currentNameInput]);
            if (len > 0) game->playerNames[game->currentNameInput][len - 1] = '\0';
        }
        else if (event->key.keysym.sym == SDLK_RETURN) {
            if (strlen(game->playerNames[game->currentNameInput]) > 0) {
                if (game->currentNameInput == 0) {
                    game->currentNameInput = 1; 
                } else {
                    game->currentState = STATE_PLAYING;
                    game->gameStartTime = SDL_GetTicks(); 
                    game->turnStartTime = SDL_GetTicks(); 
                    SDL_StopTextInput(); 
                    Audio_StopMusic();
                }
            }
        }
    } else if (event->type == SDL_TEXTINPUT) {
        if (strlen(game->playerNames[game->currentNameInput]) < MAX_NAME_LENGTH - 1) {
            if (isalpha(event->text.text[0]) || isdigit(event->text.text[0]) || isspace(event->text.text[0])) {
                 strcat(game->playerNames[game->currentNameInput], event->text.text);
            }
        }
    }
}

void Input_HandleGame(SDL_Event *event, GameState *game, AppContext *app) {
    if (event->type == SDL_QUIT) game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION) {
        game->giveUpButton.isHovered = Input_IsMouseOverButton(event, &game->giveUpButton);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (Input_IsMouseOverButton(event, &game->giveUpButton)) {
            Audio_StopMusic();
            Audio_PlaySound(app->sfxWin); 
            game->currentState = STATE_GAME_OVER;
            SDL_StopTextInput();
            return;
        }
        int x = event->button.x / TILE_SIZE;
        int y = event->button.y / TILE_SIZE;
        if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE && !game->grid[x][y]) {
            game->isTileSelected = true;
            game->selectedX = x;
            game->selectedY = y;
            SDL_StartTextInput(); 
        }
    } else if (event->type == SDL_KEYDOWN) {
        if (game->isTileSelected && event->key.keysym.sym == SDLK_RETURN && game->currentLetter) {
            game->grid[game->selectedX][game->selectedY] = game->currentLetter;
            Audio_PlaySound(app->sfxPlaceTile);
            
            int score = Logic_CheckAndScore(game, game->selectedX, game->selectedY, game->currentLetter);
            if (score > 0) {
                Audio_PlaySound(app->sfxValidWord);
                game->scores[game->currentPlayer] += score;
            }
            game->currentPlayer = (game->currentPlayer + 1) % 2;
            game->isTileSelected = false;
            game->currentLetter = '\0';
            game->turnStartTime = SDL_GetTicks(); 
            SDL_StopTextInput(); 
        }
    } else if (event->type == SDL_TEXTINPUT) {
        if (game->isTileSelected && strlen(event->text.text) == 1 && isalpha(event->text.text[0])) {
            game->currentLetter = toupper(event->text.text[0]);
        }
    }
}

void Input_HandleGameOver(SDL_Event *event, GameState *game) {
    if (event->type == SDL_QUIT) game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION) {
        game->playAgainButton.isHovered = Input_IsMouseOverButton(event, &game->playAgainButton);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN && Input_IsMouseOverButton(event, &game->playAgainButton)) {
        game->currentState = STATE_RESTART;
    }
    else if (event->type == SDL_KEYDOWN) game->currentState = STATE_QUIT;
}