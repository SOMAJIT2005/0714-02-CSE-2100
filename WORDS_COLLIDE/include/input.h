#ifndef INPUT_H
#define INPUT_H

#include "types.h"

void Input_HandleSplash(SDL_Event *event, GameState *game);
void Input_HandleNames(SDL_Event *event, GameState *game, AppContext *app);
void Input_HandleGame(SDL_Event *event, GameState *game, AppContext *app);
void Input_HandleGameOver(SDL_Event *event, GameState *game);

#endif // INPUT_H