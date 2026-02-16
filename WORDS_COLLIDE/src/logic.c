#include "../include/logic.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Encapsulated purely inside this module (SRP)
static char s_giantDictionary[MAX_WORDS_IN_DICT][MAX_WORD_LENGTH];
static int s_actualWordCount = 0;

static const char* s_bonusWords[MAX_BONUS_WORDS] = {
    "BONUS", "EXTRA", "SPECIAL", "MAGIC", "EYE", "GREEN"
};
static const int s_bonusWordCount = 6; 

static int Logic_CompareStrings(const void *a, const void *b) {
    return strcmp((const char *)a, (const char *)b);
}

void Logic_LoadDictionary(void) {
    FILE *file = fopen("assets/Dictionary.txt", "r");
    if (!file) {
        printf("FATAL ERROR: assets/Dictionary.txt not found!\n");
        return;
    }
    printf("Loading dictionary...\n");
    while (s_actualWordCount < MAX_WORDS_IN_DICT && 
           fscanf(file, "%s", s_giantDictionary[s_actualWordCount]) == 1) 
    {
        for(int i = 0; s_giantDictionary[s_actualWordCount][i]; i++){
            s_giantDictionary[s_actualWordCount][i] = toupper(s_giantDictionary[s_actualWordCount][i]);
        }
        s_actualWordCount++;
    }
    fclose(file);
    printf("Loaded %d words into memory.\n", s_actualWordCount);
}

bool Logic_IsValidWord(const char *word) {
    char tempWord[MAX_WORD_LENGTH];
    strcpy(tempWord, word);
    for(int i = 0; tempWord[i]; i++){
      tempWord[i] = toupper(tempWord[i]);
    }
    void *result = bsearch(tempWord, s_giantDictionary, s_actualWordCount, MAX_WORD_LENGTH, Logic_CompareStrings); 
    return (result != NULL);
}

static bool Logic_IsWordAlreadyScored(GameState *game, const char *word) {
    for (int i = 0; i < game->scoredWordCount; i++) {
        if (strcasecmp(word, game->scoredWords[i]) == 0) return true;
    }
    return false;
}

static int Logic_CalculateWordScore(const char *word) {
    int length = 0;
    for (int i = 0; word[i]; i++) {
        if (isalpha(word[i])) length++;
    }
    return length;
}

static bool Logic_IsBonusWord(const char *word) {
    char upperWord[MAX_WORD_LENGTH];
    strcpy(upperWord, word);
    for(int i = 0; upperWord[i]; i++) {
      upperWord[i] = toupper(upperWord[i]);
    }
    for (int i = 0; i < s_bonusWordCount; i++) {
        if (strcmp(upperWord, s_bonusWords[i]) == 0) return true; 
    }
    return false;
}

int Logic_CheckAndScore(GameState *game, int x, int y, char letter) {
    int totalScore = 0;
    
    // Horizontal Check
    char hWord[MAX_WORD_LENGTH] = "";
    int hStart = x;
    while (hStart > 0 && game->grid[hStart - 1][y] != '\0') hStart--;
    int hLen = 0;
    for (int i = hStart; i < GRID_SIZE && (game->grid[i][y] != '\0' || i == x); i++) {
        if (hLen >= MAX_WORD_LENGTH - 1) break;
        hWord[hLen++] = (i == x) ? letter : game->grid[i][y];
    }
    hWord[hLen] = '\0';
    
    if (hLen > 1 && Logic_IsValidWord(hWord) && !Logic_IsWordAlreadyScored(game, hWord)) {
        int score = Logic_CalculateWordScore(hWord);
        if (Logic_IsBonusWord(hWord)) score += BONUS_WORD_POINTS;
        totalScore += score;
        if (game->scoredWordCount < MAX_SCORED_WORDS) {
            strcpy(game->scoredWords[game->scoredWordCount++], hWord);
        }
    }

    // Vertical Check
    char vWord[MAX_WORD_LENGTH] = "";
    int vStart = y;
    while (vStart > 0 && game->grid[x][vStart - 1] != '\0') vStart--;
    int vLen = 0;
    for (int i = vStart; i < GRID_SIZE && (game->grid[x][i] != '\0' || i == y); i++) {
        if (vLen >= MAX_WORD_LENGTH - 1) break;
        vWord[vLen++] = (i == y) ? letter : game->grid[x][i];
    }
    vWord[vLen] = '\0';
    
    if (vLen > 1 && Logic_IsValidWord(vWord) && !Logic_IsWordAlreadyScored(game, vWord)) {
        int score = Logic_CalculateWordScore(vWord);
        if (Logic_IsBonusWord(vWord)) score += BONUS_WORD_POINTS;
        totalScore += score;
        if (game->scoredWordCount < MAX_SCORED_WORDS) {
            strcpy(game->scoredWords[game->scoredWordCount++], vWord);
        }
    }
    
    return totalScore;
}