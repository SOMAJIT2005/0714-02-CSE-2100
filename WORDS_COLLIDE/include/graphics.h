#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"

void Graphics_RenderSplashScreen(AppContext *app, GameState *game);
void Graphics_RenderModeMenu(AppContext *app, GameState *game);
void Graphics_RenderOpponentMenu(AppContext *app, GameState *game);
void Graphics_RenderBotMenu(AppContext *app, GameState *game);
void Graphics_RenderMultiplayerMenu(AppContext *app, GameState *game);
void Graphics_RenderIPInput(AppContext *app, GameState *game);
void Graphics_RenderNameInput(AppContext *app, GameState *game);
void Graphics_RenderTutorial(AppContext *app, GameState *game);
void Graphics_RenderGame(AppContext *app, GameState *game);
void Graphics_RenderGameOver(AppContext *app, GameState *game);
void Graphics_RenderWaitingForOpponent(AppContext *app, GameState *game);

void Graphics_RenderMysteryTutorial(AppContext *app, GameState *game);
void Graphics_RenderMystery(AppContext *app, GameState *game);
void Graphics_RenderMysteryGameOver(AppContext *app, GameState *game);

#endif