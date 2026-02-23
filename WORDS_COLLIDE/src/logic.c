#include "../include/logic.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static char loadedDictionary[MAX_WORDS_IN_DICT][MAX_WORD_LENGTH];
static int totalLoadedWords = 0;

static const char* specialBonusWords[MAX_BONUS_WORDS] = {
    "BONUS", "EXTRA", "SPECIAL", "MAGIC", "EYE", "GREEN"
};
static const int totalBonusWords = 6; 

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
    
    while (totalLoadedWords < MAX_WORDS_IN_DICT && 
           fscanf(file, "%s", loadedDictionary[totalLoadedWords]) == 1) 
    {
        // Convert the word to uppercase for consistent checking
        for (int index = 0; loadedDictionary[totalLoadedWords][index] != '\0'; index++) {
            loadedDictionary[totalLoadedWords][index] = toupper(loadedDictionary[totalLoadedWords][index]);
        }
        totalLoadedWords++;
    }
    
    fclose(file);
    printf("Loaded %d words into memory.\n", totalLoadedWords);
}

bool Logic_IsValidWord(const char *word) {
    char uppercaseWord[MAX_WORD_LENGTH];
    strcpy(uppercaseWord, word);
    
    for (int index = 0; uppercaseWord[index] != '\0'; index++) {
      uppercaseWord[index] = toupper(uppercaseWord[index]);
    }
    
    void *searchResult = bsearch(uppercaseWord, loadedDictionary, totalLoadedWords, MAX_WORD_LENGTH, Logic_CompareStrings); 
    return (searchResult != NULL);
}

static bool Logic_IsWordAlreadyScored(GameState *game, const char *word) {
    for (int i = 0; i < game->scoredWordCount; i++) {
        if (strcasecmp(word, game->scoredWords[i]) == 0) {
            return true;
        }
    }
    return false;
}

static int Logic_CalculateWordScore(const char *word) {
    int lengthScore = 0;
    for (int i = 0; word[i] != '\0'; i++) {
        if (isalpha(word[i])) {
            lengthScore++;
        }
    }
    return lengthScore;
}

static bool Logic_IsBonusWord(const char *word) {
    char uppercaseWord[MAX_WORD_LENGTH];
    strcpy(uppercaseWord, word);
    
    for (int i = 0; uppercaseWord[i] != '\0'; i++) {
      uppercaseWord[i] = toupper(uppercaseWord[i]);
    }
    
    for (int i = 0; i < totalBonusWords; i++) {
        if (strcmp(uppercaseWord, specialBonusWords[i]) == 0) {
            return true; 
        }
    }
    return false;
}

int Logic_CheckAndScore(GameState *game, int targetCol, int targetRow, char placedLetter) {
    int totalScoreGained = 0;
    
    // -------------------------
    // 1. Horizontal Word Check
    // -------------------------
    char horizontalWord[MAX_WORD_LENGTH] = "";
    int startColumn = targetCol;
    
    // Find where the word starts by looking left
    while (startColumn > 0 && game->grid[startColumn - 1][targetRow] != '\0') {
        startColumn--;
    }
    
    // Read the word left-to-right
    int currentLength = 0;
    for (int col = startColumn; col < GRID_SIZE; col++) {
        char currentTile = (col == targetCol) ? placedLetter : game->grid[col][targetRow];
        
        if (currentTile == '\0') break; // Blank space ends the word
        
        if (currentLength < MAX_WORD_LENGTH - 1) {
            horizontalWord[currentLength++] = currentTile;
        }
    }
    horizontalWord[currentLength] = '\0';
    
    // Validate and score horizontal word
    if (currentLength > 1 && Logic_IsValidWord(horizontalWord) && !Logic_IsWordAlreadyScored(game, horizontalWord)) {
        int wordScore = Logic_CalculateWordScore(horizontalWord);
        if (Logic_IsBonusWord(horizontalWord)) {
            wordScore += BONUS_WORD_POINTS;
        }
        
        totalScoreGained += wordScore;
        
        if (game->scoredWordCount < MAX_SCORED_WORDS) {
            strcpy(game->scoredWords[game->scoredWordCount++], horizontalWord);
        }
    }

    // -----------------------
    // 2. Vertical Word Check
    // -----------------------
    char verticalWord[MAX_WORD_LENGTH] = "";
    int startRow = targetRow;
    
    // Find where the word starts by looking up
    while (startRow > 0 && game->grid[targetCol][startRow - 1] != '\0') {
        startRow--;
    }
    
    // Read the word top-to-bottom
    currentLength = 0;
    for (int row = startRow; row < GRID_SIZE; row++) {
        char currentTile = (row == targetRow) ? placedLetter : game->grid[targetCol][row];
        
        if (currentTile == '\0') break; // Blank space ends the word
        
        if (currentLength < MAX_WORD_LENGTH - 1) {
            verticalWord[currentLength++] = currentTile;
        }
    }
    verticalWord[currentLength] = '\0';
    
    // Validate and score vertical word
    if (currentLength > 1 && Logic_IsValidWord(verticalWord) && !Logic_IsWordAlreadyScored(game, verticalWord)) {
        int wordScore = Logic_CalculateWordScore(verticalWord);
        if (Logic_IsBonusWord(verticalWord)) {
            wordScore += BONUS_WORD_POINTS;
        }
        
        totalScoreGained += wordScore;
        
        if (game->scoredWordCount < MAX_SCORED_WORDS) {
            strcpy(game->scoredWords[game->scoredWordCount++], verticalWord);
        }
    }
    
    return totalScoreGained;
}