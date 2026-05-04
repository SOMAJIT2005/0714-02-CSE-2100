#include "../include/input.h"
#include "../include/audio.h"
#include "../include/logic.h"
#include "../include/network.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

static bool Input_IsMouseOverButton(SDL_Event *event, Button *button)
{
    int x = 0, y = 0;
    if (event->type == SDL_MOUSEMOTION)
    {
        x = event->motion.x;
        y = event->motion.y;
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        x = event->button.x;
        y = event->button.y;
    }
    else
        return false;
    return (x >= button->rect.x && x <= button->rect.x + button->rect.w && y >= button->rect.y && y <= button->rect.y + button->rect.h);
}

void Input_HandleSplash(SDL_Event *event, GameState *game)
{
    if (event->type == SDL_QUIT)
        game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION)
    {
        game->playWordsCollideButton.isHovered = Input_IsMouseOverButton(event, &game->playWordsCollideButton);
        game->playMysteryButton.isHovered = Input_IsMouseOverButton(event, &game->playMysteryButton);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (Input_IsMouseOverButton(event, &game->playWordsCollideButton))
            game->currentState = STATE_MODE_MENU;
        else if (Input_IsMouseOverButton(event, &game->playMysteryButton))
            game->currentState = STATE_MYSTERY_TUTORIAL;
    }
}
void Input_HandleModeMenu(SDL_Event *event, GameState *game)
{
    if (event->type == SDL_QUIT)
        game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION)
    {
        game->classicModeButton.isHovered = Input_IsMouseOverButton(event, &game->classicModeButton);
        game->blitzModeButton.isHovered = Input_IsMouseOverButton(event, &game->blitzModeButton);
        game->backButton.isHovered = Input_IsMouseOverButton(event, &game->backButton);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (Input_IsMouseOverButton(event, &game->classicModeButton))
        {
            game->gameMode = 0;
            game->currentState = STATE_OPPONENT_MENU;
        }
        else if (Input_IsMouseOverButton(event, &game->blitzModeButton))
        {
            game->gameMode = 1;
            game->currentState = STATE_OPPONENT_MENU;
        }
        else if (Input_IsMouseOverButton(event, &game->backButton))
            game->currentState = STATE_SPLASH;
    }
}
void Input_HandleOpponentMenu(SDL_Event *event, GameState *game)
{
    if (event->type == SDL_QUIT)
        game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION)
    {
        game->singlePlayerButton.isHovered = Input_IsMouseOverButton(event, &game->singlePlayerButton);
        game->startGameButton.isHovered = Input_IsMouseOverButton(event, &game->startGameButton);
        game->multiplayerButton.isHovered = Input_IsMouseOverButton(event, &game->multiplayerButton);
        game->backButton.isHovered = Input_IsMouseOverButton(event, &game->backButton);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (Input_IsMouseOverButton(event, &game->singlePlayerButton))
            game->currentState = STATE_BOT_MENU;
        else if (Input_IsMouseOverButton(event, &game->startGameButton))
        {
            game->isSinglePlayer = false;
            game->currentState = STATE_GET_NAMES;
            SDL_StartTextInput();
        }
        else if (Input_IsMouseOverButton(event, &game->multiplayerButton))
        {
            game->isSinglePlayer = false;
            game->currentState = STATE_MULTIPLAYER_MENU;
        }
        else if (Input_IsMouseOverButton(event, &game->backButton))
            game->currentState = STATE_MODE_MENU;
    }
}
void Input_HandleBotMenu(SDL_Event *event, GameState *game)
{
    if (event->type == SDL_QUIT)
        game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION)
    {
        game->botEasyButton.isHovered = Input_IsMouseOverButton(event, &game->botEasyButton);
        game->botHardButton.isHovered = Input_IsMouseOverButton(event, &game->botHardButton);
        game->backButton.isHovered = Input_IsMouseOverButton(event, &game->backButton);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (Input_IsMouseOverButton(event, &game->botEasyButton))
        {
            game->isSinglePlayer = true;
            game->botDifficulty = 0;
            strcpy(game->playerNames[1], "Scribble Sprite");
            game->currentState = STATE_GET_NAMES;
            SDL_StartTextInput();
        }
        else if (Input_IsMouseOverButton(event, &game->botHardButton))
        {
            game->isSinglePlayer = true;
            game->botDifficulty = 1;
            strcpy(game->playerNames[1], "Lexicon Leviathan");
            game->currentState = STATE_GET_NAMES;
            SDL_StartTextInput();
        }
        else if (Input_IsMouseOverButton(event, &game->backButton))
            game->currentState = STATE_OPPONENT_MENU;
    }
}
void Input_HandleMultiplayerMenu(SDL_Event *event, GameState *game)
{
    if (event->type == SDL_QUIT)
        game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION)
    {
        game->hostGameButton.isHovered = Input_IsMouseOverButton(event, &game->hostGameButton);
        game->joinGameButton.isHovered = Input_IsMouseOverButton(event, &game->joinGameButton);
        game->backButton.isHovered = Input_IsMouseOverButton(event, &game->backButton);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (Input_IsMouseOverButton(event, &game->hostGameButton))
        {
            if (Network_HostGame(9021)) // FIX: Port updated to 9021
            {
                game->isMultiplayer = true;
                game->localPlayerId = 0;
                game->currentNameInput = 0;
                game->currentState = STATE_GET_NAMES;
                SDL_StartTextInput();
            }
        }
        else if (Input_IsMouseOverButton(event, &game->joinGameButton))
        {
            char foundIP[64] = "";
            bool hostFound = false;
            for (int i = 0; i < 25; i++)
            {
                if (Network_ListenForHost(foundIP))
                {
                    hostFound = true;
                    break;
                }
                SDL_Delay(100);
            }
            if (hostFound)
            {
                if (Network_JoinGame(foundIP, 9021)) // FIX: Port updated to 9021
                {
                    game->isMultiplayer = true;
                    game->localPlayerId = 1;
                    game->currentNameInput = 1;
                    game->currentState = STATE_GET_NAMES;
                    SDL_StartTextInput();
                }
            }
            else
            {
                memset(game->targetIP, 0, sizeof(game->targetIP));
                game->currentState = STATE_ENTER_IP;
                SDL_StartTextInput();
            }
        }
        else if (Input_IsMouseOverButton(event, &game->backButton))
            game->currentState = STATE_OPPONENT_MENU;
    }
}
void Input_HandleIPInput(SDL_Event *event, GameState *game)
{
    if (event->type == SDL_QUIT)
        game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION)
        game->backButton.isHovered = Input_IsMouseOverButton(event, &game->backButton);
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (Input_IsMouseOverButton(event, &game->backButton))
        {
            game->currentState = STATE_MULTIPLAYER_MENU;
            SDL_StopTextInput();
        }
        else 
        {
            SDL_StartTextInput(); // FIX: Clicking brings the keyboard back
        }
    }
    else if (event->type == SDL_KEYDOWN)
    {
        if (event->key.keysym.sym == SDLK_BACKSPACE)
        {
            int len = strlen(game->targetIP);
            if (len > 0)
                game->targetIP[len - 1] = '\0';
        }
        else if (event->key.keysym.sym == SDLK_RETURN)
        {
            if (strlen(game->targetIP) > 0)
            {
                if (Network_JoinGame(game->targetIP, 9021)) // FIX: Port updated to 9021
                {
                    game->isMultiplayer = true;
                    game->localPlayerId = 1;
                    game->currentNameInput = 1;
                    game->currentState = STATE_GET_NAMES;
                }
                else
                    memset(game->targetIP, 0, sizeof(game->targetIP));
            }
        }
    }
    else if (event->type == SDL_TEXTINPUT)
    {
        if (strlen(game->targetIP) < 15)
        {
            char c = event->text.text[0];
            if (isdigit(c) || c == '.')
                strcat(game->targetIP, event->text.text);
        }
    }
}
void Input_HandleNames(SDL_Event *event, GameState *game, AppContext *app)
{
    if (event->type == SDL_QUIT)
        game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEBUTTONDOWN) 
    {
        SDL_StartTextInput(); // FIX: Clicking brings the keyboard back
    }
    else if (event->type == SDL_KEYDOWN)
    {
        if (event->key.keysym.sym == SDLK_BACKSPACE)
        {
            int len = strlen(game->playerNames[game->currentNameInput]);
            if (len > 0)
                game->playerNames[game->currentNameInput][len - 1] = '\0';
        }
        else if (event->key.keysym.sym == SDLK_RETURN)
        {
            if (strlen(game->playerNames[game->currentNameInput]) > 0)
            {
                if (game->isMultiplayer)
                {
                    game->playerReady[game->localPlayerId] = true;
                    char msg[256];
                    sprintf(msg, "READY,%d,%s\n", game->localPlayerId, game->playerNames[game->localPlayerId]);
                    Network_SendMessage(msg);
                    
                    // FIX: THE UI TRAP! Only wait if opponent isn't ready yet.
                    int opponentId = (game->localPlayerId == 0) ? 1 : 0;
                    if (game->playerReady[opponentId]) {
                        game->currentState = STATE_PLAYING;
                        game->gameStartTime = SDL_GetTicks();
                        game->turnStartTime = SDL_GetTicks();
                    } else {
                        game->currentState = STATE_WAITING_FOR_OPPONENT;
                    }
                    
                    SDL_StopTextInput();
                }
                else
                {
                    if (game->currentNameInput == 0 && !game->isSinglePlayer)
                    {
                        game->currentNameInput = 1;
                    }
                    else
                    {
                        game->currentState = STATE_TUTORIAL;
                        game->tutorialStartTime = SDL_GetTicks();
                        SDL_StopTextInput();
                        Audio_StopMusic();
                    }
                }
            }
        }
    }
    else if (event->type == SDL_TEXTINPUT)
    {
        if (strlen(game->playerNames[game->currentNameInput]) < MAX_NAME_LENGTH - 1)
        {
            if (isalpha(event->text.text[0]) || isdigit(event->text.text[0]) || event->text.text[0] == ' ')
            {
                strcat(game->playerNames[game->currentNameInput], event->text.text);
            }
        }
    }
}
void Input_HandleTutorial(SDL_Event *event, GameState *game)
{
    if (event->type == SDL_QUIT)
        game->currentState = STATE_QUIT;
    else if ((event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_KEYDOWN) && (SDL_GetTicks() - game->tutorialStartTime > 500))
    {
        game->currentState = STATE_PLAYING;
        game->gameStartTime = SDL_GetTicks();
        game->turnStartTime = SDL_GetTicks();
    }
}

void Input_HandleGame(SDL_Event *event, GameState *game, AppContext *app)
{
    if (event->type == SDL_QUIT)
        game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION)
    {
        game->giveUpButton.isHovered = Input_IsMouseOverButton(event, &game->giveUpButton);
        game->pauseButton.isHovered = Input_IsMouseOverButton(event, &game->pauseButton);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (Input_IsMouseOverButton(event, &game->giveUpButton))
        {
            game->forfeitedPlayer = game->isMultiplayer ? game->localPlayerId : game->currentPlayer;
            strcpy(game->endReason, "Player Resigned");
            Audio_StopMusic();
            Audio_PlaySound(app->sfxWin);
            game->currentState = STATE_GAME_OVER;
            SDL_StopTextInput();
            if (game->isMultiplayer)
            {
                char msg[64];
                sprintf(msg, "GIVE_UP,%d\n", game->localPlayerId);
                Network_SendMessage(msg);
            }
            return;
        }
        if (Input_IsMouseOverButton(event, &game->pauseButton))
        {
            game->isPaused = !game->isPaused;
            if (game->isMultiplayer)
                Network_SendMessage(game->isPaused ? "PAUSE_ON\n" : "PAUSE_OFF\n");
            return;
        }
        if (game->isPaused)
        {
            game->isPaused = false;
            if (game->isMultiplayer)
                Network_SendMessage("PAUSE_OFF\n");
            return;
        }
        if (game->isSinglePlayer && game->currentPlayer == 1)
            return;
        if (game->isMultiplayer && game->currentPlayer != game->localPlayerId)
            return;

        int startX = (WINDOW_WIDTH - (GRID_COLS * TILE_SIZE)) / 2;
        int startY = 45 + (WINDOW_HEIGHT - 60 - 45 - (GRID_ROWS * TILE_SIZE)) / 2;
        int mx = event->button.x;
        int my = event->button.y;

        if (mx >= startX && mx < startX + GRID_COLS * TILE_SIZE && my >= startY && my < startY + GRID_ROWS * TILE_SIZE)
        {
            int x = (mx - startX) / TILE_SIZE;
            int y = (my - startY) / TILE_SIZE;
            if (x >= 0 && x < GRID_COLS && y >= 0 && y < GRID_ROWS && !game->grid[x][y])
            {
                game->isTileSelected = true;
                game->selectedX = x;
                game->selectedY = y;
                SDL_StartTextInput();
            }
        }
    }
    else if (event->type == SDL_KEYDOWN)
    {
        if (game->isPaused)
        {
            game->isPaused = false;
            if (game->isMultiplayer)
                Network_SendMessage("PAUSE_OFF\n");
            return;
        }
        if (game->isSinglePlayer && game->currentPlayer == 1)
            return;
        if (game->isMultiplayer && game->currentPlayer != game->localPlayerId)
            return;
        if (game->isTileSelected && event->key.keysym.sym == SDLK_RETURN && game->currentLetter)
        {
            if (game->isMultiplayer)
            {
                char msg[256];
                sprintf(msg, "MOVE,%d,%d,%c\n", game->selectedX, game->selectedY, game->currentLetter);
                Network_SendMessage(msg);
            }
            game->totalTurnTime[game->currentPlayer] += (SDL_GetTicks() - game->turnStartTime);
            game->turnsPlayed[game->currentPlayer]++;
            game->grid[game->selectedX][game->selectedY] = game->currentLetter;
            Audio_PlaySound(app->sfxPlaceTile);
            int score = Logic_CheckAndScore(game, game->selectedX, game->selectedY, game->currentLetter);
            if (score > 0)
            {
                Audio_PlaySound(app->sfxValidWord);
                game->scores[game->currentPlayer] += score;
            }
            game->currentPlayer = (game->currentPlayer + 1) % 2;
            game->isTileSelected = false;
            game->currentLetter = '\0';
            game->turnStartTime = SDL_GetTicks();
            SDL_StopTextInput();
            Logic_CheckWinConditions(game, app);
        }
    }
    else if (event->type == SDL_TEXTINPUT)
    {
        if (game->isPaused)
            return;
        if (game->isSinglePlayer && game->currentPlayer == 1)
            return;
        if (game->isMultiplayer && game->currentPlayer != game->localPlayerId)
            return;
        if (game->isTileSelected && strlen(event->text.text) == 1)
        {
            char c = toupper(event->text.text[0]);
            if (c >= 'A' && c <= 'Z')
                game->currentLetter = c;
        }
    }
}

void Input_HandleGameOver(SDL_Event *event, GameState *game)
{
    if (event->type == SDL_QUIT)
        game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION)
        game->playAgainButton.isHovered = Input_IsMouseOverButton(event, &game->playAgainButton);
    else if (event->type == SDL_MOUSEBUTTONDOWN && Input_IsMouseOverButton(event, &game->playAgainButton))
        game->currentState = STATE_RESTART;
    else if (event->type == SDL_KEYDOWN)
        game->currentState = STATE_QUIT;
}
void Input_HandleMysteryTutorial(SDL_Event *event, GameState *game)
{
    if (event->type == SDL_QUIT)
        game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_KEYDOWN)
    {
        Logic_SetupMystery(game);
        game->currentState = STATE_MYSTERY_PLAYING;
        SDL_StartTextInput();
        Audio_StopMusic();
    }
}

void Input_HandleMysteryPlaying(SDL_Event *event, GameState *game, AppContext *app)
{
    if (event->type == SDL_QUIT)
        game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION)
        game->backButton.isHovered = Input_IsMouseOverButton(event, &game->backButton);
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (Input_IsMouseOverButton(event, &game->backButton))
        {
            game->currentState = STATE_RESTART;
            SDL_StopTextInput();
        }
        else
        {
            SDL_StartTextInput();
        }
    }
    else if (event->type == SDL_KEYDOWN)
    {
        if (event->key.keysym.sym == SDLK_BACKSPACE && game->mysteryCol > 0)
        {
            game->mysteryCol--;
            game->mysteryGrid[game->mysteryRow][game->mysteryCol] = '\0';
        }
        else if (event->key.keysym.sym == SDLK_RETURN && game->mysteryCol == MYSTERY_COLS)
        {
            if (Logic_CheckMysteryRow(game))
            {
                Audio_PlaySound(app->sfxPlaceTile);
                if (game->mysteryState > 0)
                {
                    game->currentState = STATE_MYSTERY_GAMEOVER;
                    SDL_StopTextInput();
                }
            }
            else
            {
                Audio_PlaySound(app->sfxWarning);
                game->mysteryCol = 0;
                for (int i = 0; i < MYSTERY_COLS; i++)
                    game->mysteryGrid[game->mysteryRow][i] = '\0';
            }
        }
    }
    else if (event->type == SDL_TEXTINPUT)
    {
        if (game->mysteryCol < MYSTERY_COLS && isalpha(event->text.text[0]))
        {
            game->mysteryGrid[game->mysteryRow][game->mysteryCol] = toupper(event->text.text[0]);
            game->mysteryCol++;
        }
    }
}

void Input_HandleMysteryGameOver(SDL_Event *event, GameState *game)
{
    if (event->type == SDL_QUIT)
        game->currentState = STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION)
    {
        game->playAgainButton.isHovered = Input_IsMouseOverButton(event, &game->playAgainButton);
        game->backButton.isHovered = Input_IsMouseOverButton(event, &game->backButton);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (Input_IsMouseOverButton(event, &game->playAgainButton))
        {
            Logic_SetupMystery(game);
            game->currentState = STATE_MYSTERY_PLAYING;
            SDL_StartTextInput();
        }
        else if (Input_IsMouseOverButton(event, &game->backButton))
            game->currentState = STATE_RESTART;
    }
}