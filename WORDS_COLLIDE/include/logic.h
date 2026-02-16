#ifndef LOGIC_H
#define LOGIC_H

#include "types.h"

void Logic_LoadDictionary(void);
bool Logic_IsValidWord(const char *word);
int Logic_CheckAndScore(GameState *game, int x, int y, char letter);

#endif // LOGIC_H