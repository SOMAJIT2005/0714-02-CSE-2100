#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"

void Graphics_RenderSplashScreen(AppContext *app, GameState *game);
void Graphics_RenderMultiplayerMenu(AppContext *app, GameState *game);
void Graphics_RenderIPInput(AppContext *app, GameState *game); // <-- NEW
void Graphics_RenderNameInput(AppContext *app, GameState *game);
void Graphics_RenderGame(AppContext *app, GameState *game);
void Graphics_RenderGameOver(AppContext *app, GameState *game);

#endif // GRAPHICS_H