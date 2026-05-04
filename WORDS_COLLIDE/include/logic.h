#ifndef LOGIC_H
#define LOGIC_H

#include "types.h"

#define MAX_WORDS_IN_DICT 200000
#define MAX_BONUS_WORDS 6

void Logic_LoadDictionary(void);
bool Logic_IsValidWord(const char *word);
void Logic_SetupMystery(GameState *game);
bool Logic_CheckMysteryRow(GameState *game);
int Logic_CheckAndScore(GameState *game, int targetCol, int targetRow, char placedLetter);
void Logic_BotMove(GameState *game, AppContext *app);
void Logic_CheckWinConditions(GameState *game, AppContext *app);

// New UI and Network additions
void Logic_InitGameState(GameState *game);
void Logic_HandleNetworkMessage(GameState *game, AppContext *app, char *msg);

#endif