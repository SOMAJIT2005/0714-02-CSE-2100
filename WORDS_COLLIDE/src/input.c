#include "../include/input.h"
#include "../include/audio.h"
#include "../include/logic.h"
#include <string.h>
#include <ctype.h>

static bool Input_IsMouseOverButton(SDL_Event *event, Button *button) {
    if (event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = event->motion.x;
        int mouseY = event->motion.y;
        
        return (mouseX >= button->rect.x && mouseX <= button->rect.x + button->rect.w &&
                mouseY >= button->rect.y && mouseY <= button->rect.y + button->rect.h);
    }
    return false;
}

void Input_HandleSplash(SDL_Event *event, GameState *game) {
    if (event->type == SDL_QUIT) {
        game->currentState = STATE_QUIT;
    } 
    else if (event->type == SDL_MOUSEMOTION) {
        game->startGameButton.isHovered = Input_IsMouseOverButton(event, &game->startGameButton);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (Input_IsMouseOverButton(event, &game->startGameButton)) {
            game->currentState = STATE_GET_NAMES;
            SDL_StartTextInput(); 
        }
    }
}

void Input_HandleNames(SDL_Event *event, GameState *game, AppContext *app) {
    if (event->type == SDL_QUIT) {
        game->currentState = STATE_QUIT;
    } 
    else if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_BACKSPACE) {
            int currentNameLength = strlen(game->playerNames[game->currentNameInput]);
            if (currentNameLength > 0) {
                game->playerNames[game->currentNameInput][currentNameLength - 1] = '\0';
            }
        }
        else if (event->key.keysym.sym == SDLK_RETURN) {
            bool hasTypedName = strlen(game->playerNames[game->currentNameInput]) > 0;
            
            if (hasTypedName) {
                if (game->currentNameInput == 0) {
                    game->currentNameInput = 1; // Move to player 2
                } else {
                    // Both names entered, start the game
                    game->currentState = STATE_PLAYING;
                    game->gameStartTime = SDL_GetTicks(); 
                    game->turnStartTime = SDL_GetTicks(); 
                    SDL_StopTextInput(); 
                    Audio_StopMusic();
                }
            }
        }
    } 
    else if (event->type == SDL_TEXTINPUT) {
        if (strlen(game->playerNames[game->currentNameInput]) < MAX_NAME_LENGTH - 1) {
            char typedCharacter = event->text.text[0];
            if (isalpha(typedCharacter) || isdigit(typedCharacter) || isspace(typedCharacter)) {
                 strcat(game->playerNames[game->currentNameInput], event->text.text);
            }
        }
    }
}

void Input_HandleGame(SDL_Event *event, GameState *game, AppContext *app) {
    if (event->type == SDL_QUIT) {
        game->currentState = STATE_QUIT;
    } 
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
        
        int clickedColumn = event->button.x / TILE_SIZE;
        int clickedRow    = event->button.y / TILE_SIZE;
        bool isInsideGrid = (clickedColumn >= 0 && clickedColumn < GRID_SIZE && clickedRow >= 0 && clickedRow < GRID_SIZE);
        bool isTileEmpty  = (game->grid[clickedColumn][clickedRow] == '\0');
        
        if (isInsideGrid && isTileEmpty) {
            game->isTileSelected = true;
            game->selectedX = clickedColumn;
            game->selectedY = clickedRow;
            SDL_StartTextInput(); 
        }
    } 
    else if (event->type == SDL_KEYDOWN) {
        bool hasPressedEnter = (event->key.keysym.sym == SDLK_RETURN);
        bool hasLetterReady  = (game->currentLetter != '\0');
        
        if (game->isTileSelected && hasPressedEnter && hasLetterReady) {
            // Commit the letter to the grid
            game->grid[game->selectedX][game->selectedY] = game->currentLetter;
            Audio_PlaySound(app->sfxPlaceTile);
            
            int scoreGained = Logic_CheckAndScore(game, game->selectedX, game->selectedY, game->currentLetter);
            if (scoreGained > 0) {
                Audio_PlaySound(app->sfxValidWord);
                game->scores[game->currentPlayer] += scoreGained;
            }
            
            // Switch turns
            game->currentPlayer = (game->currentPlayer + 1) % 2;
            game->isTileSelected = false;
            game->currentLetter = '\0';
            game->turnStartTime = SDL_GetTicks(); 
            SDL_StopTextInput(); 
        }
    } 
    else if (event->type == SDL_TEXTINPUT) {
        bool isSingleCharacter = strlen(event->text.text) == 1;
        char typedCharacter = event->text.text[0];
        
        if (game->isTileSelected && isSingleCharacter && isalpha(typedCharacter)) {
            game->currentLetter = toupper(typedCharacter);
        }
    }
}

void Input_HandleGameOver(SDL_Event *event, GameState *game) {
    if (event->type == SDL_QUIT) {
        game->currentState = STATE_QUIT;
    } 
    else if (event->type == SDL_MOUSEMOTION) {
        game->playAgainButton.isHovered = Input_IsMouseOverButton(event, &game->playAgainButton);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (Input_IsMouseOverButton(event, &game->playAgainButton)) {
            game->currentState = STATE_RESTART;
        }
    }
    else if (event->type == SDL_KEYDOWN) {
        game->currentState = STATE_QUIT;
    }
}