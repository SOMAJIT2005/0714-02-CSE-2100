#include "Logic.hpp"
#include <cstdio>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <vector>
#include <iostream>

static std::vector<std::string> loadedDictionary;
static const std::vector<std::string> specialBonusWords = {"BONUS","EXTRA","SPECIAL","MAGIC","EYE","GREEN"};

void Logic::loadDictionary() {
    loadedDictionary.clear();
    FILE* f = std::fopen("assets/Dictionary.txt", "r");
    if (!f) {
        std::cerr << "FATAL ERROR: assets/Dictionary.txt not found!\n";
        return;
    }
    char buf[256];
    while (std::fscanf(f, "%255s", buf) == 1 && loadedDictionary.size() < static_cast<size_t>(MAX_WORDS_IN_DICT)) {
        std::string s(buf);
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        loadedDictionary.push_back(s);
    }
    std::fclose(f);
    std::sort(loadedDictionary.begin(), loadedDictionary.end());
    std::cout << "Loaded " << loadedDictionary.size() << " words into memory.\n";
}

bool Logic::isValidWord(const std::string& word) {
    std::string up = word;
    std::transform(up.begin(), up.end(), up.begin(), ::toupper);
    return std::binary_search(loadedDictionary.begin(), loadedDictionary.end(), up);
}

static bool isWordAlreadyScored(GameState& game, const std::string& word) {
    for (int i = 0; i < game.scoredWordCount; ++i) {
        const std::string& s = game.scoredWords[i];
        if (s.size() == word.size()) {
            bool equal = true;
            for (size_t k = 0; k < s.size(); ++k) {
                if (std::toupper(static_cast<unsigned char>(s[k])) != std::toupper(static_cast<unsigned char>(word[k]))) {
                    equal = false;
                    break;
                }
            }
            if (equal) return true;
        } else {
            // case-insensitive compare for different lengths will be false
            std::string s_up = s;
            std::transform(s_up.begin(), s_up.end(), s_up.begin(), ::toupper);
            std::string w_up = word;
            std::transform(w_up.begin(), w_up.end(), w_up.begin(), ::toupper);
            if (s_up == w_up) return true;
        }
    }
    return false;
}

static int calculateWordScore(const std::string& word) {
    int lengthScore = 0;
    for (char c : word) if (std::isalpha(static_cast<unsigned char>(c))) ++lengthScore;
    return lengthScore;
}

static bool isBonusWord(const std::string& word) {
    std::string up = word;
    std::transform(up.begin(), up.end(), up.begin(), ::toupper);
    for (const auto &b : specialBonusWords) if (up == b) return true;
    return false;
}

int Logic::checkAndScore(GameState& game, int targetCol, int targetRow, char placedLetter) {
    int totalScoreGained = 0;

    // Horizontal
    std::string horizontal;
    int startColumn = targetCol;
    while (startColumn > 0 && game.grid[startColumn - 1][targetRow] != 0) --startColumn;
    for (int col = startColumn; col < GRID_SIZE; ++col) {
        char currentTile = (col == targetCol) ? placedLetter : game.grid[col][targetRow];
        if (currentTile == 0) break;
        if (horizontal.size() < static_cast<size_t>(MAX_WORD_LENGTH - 1)) horizontal.push_back(currentTile);
    }
    if (horizontal.size() > 1 && isValidWord(horizontal) && !isWordAlreadyScored(game, horizontal)) {
        int wordScore = calculateWordScore(horizontal);
        if (isBonusWord(horizontal)) wordScore += BONUS_WORD_POINTS;
        totalScoreGained += wordScore;
        if (game.scoredWordCount < MAX_SCORED_WORDS) {
            game.scoredWords[game.scoredWordCount++] = horizontal;
        }
    }

    // Vertical
    std::string vertical;
    int startRow = targetRow;
    while (startRow > 0 && game.grid[targetCol][startRow - 1] != 0) --startRow;
    for (int row = startRow; row < GRID_SIZE; ++row) {
        char currentTile = (row == targetRow) ? placedLetter : game.grid[targetCol][row];
        if (currentTile == 0) break;
        if (vertical.size() < static_cast<size_t>(MAX_WORD_LENGTH - 1)) vertical.push_back(currentTile);
    }
    if (vertical.size() > 1 && isValidWord(vertical) && !isWordAlreadyScored(game, vertical)) {
        int wordScore = calculateWordScore(vertical);
        if (isBonusWord(vertical)) wordScore += BONUS_WORD_POINTS;
        totalScoreGained += wordScore;
        if (game.scoredWordCount < MAX_SCORED_WORDS) {
            game.scoredWords[game.scoredWordCount++] = vertical;
        }
    }

    return totalScoreGained;
}
