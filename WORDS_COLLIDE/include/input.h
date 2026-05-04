#ifndef INPUT_H
#define INPUT_H

#include "types.h"

void Input_HandleSplash(SDL_Event *event, GameState *game);
void Input_HandleModeMenu(SDL_Event *event, GameState *game);
void Input_HandleOpponentMenu(SDL_Event *event, GameState *game);
void Input_HandleBotMenu(SDL_Event *event, GameState *game);
void Input_HandleMultiplayerMenu(SDL_Event *event, GameState *game);
void Input_HandleIPInput(SDL_Event *event, GameState *game);
void Input_HandleNames(SDL_Event *event, GameState *game, AppContext *app);
void Input_HandleTutorial(SDL_Event *event, GameState *game);
void Input_HandleGame(SDL_Event *event, GameState *game, AppContext *app);
void Input_HandleGameOver(SDL_Event *event, GameState *game);

void Input_HandleMysteryTutorial(SDL_Event *event, GameState *game);
void Input_HandleMysteryPlaying(SDL_Event *event, GameState *game, AppContext *app);
void Input_HandleMysteryGameOver(SDL_Event *event, GameState *game);

#endif