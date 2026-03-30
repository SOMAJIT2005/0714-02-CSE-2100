#include "Game/BoardScorer.hpp"
#include <algorithm>
#include <cctype>

BoardScorer::BoardScorer(const Dictionary& dict) : dictionary(dict) {}

int BoardScorer::calculateWordScore(const std::string& word) const {
    int lengthScore = 0;
    for (char c : word) {
        if (std::isalpha(static_cast<unsigned char>(c))) ++lengthScore;
    }
    return lengthScore;
}

bool BoardScorer::isWordAlreadyScored(const GameState& game, const std::string& word) const {
    for (int i = 0; i < game.scoredWordCount; ++i) {
        std::string s_up = game.scoredWords[i];
        std::transform(s_up.begin(), s_up.end(), s_up.begin(), ::toupper);
        std::string w_up = word;
        std::transform(w_up.begin(), w_up.end(), w_up.begin(), ::toupper);
        if (s_up == w_up) return true;
    }
    return false;
}

int BoardScorer::checkAndScore(GameState& game, int targetCol, int targetRow, char placedLetter) const {
    int totalScoreGained = 0;

    // Horizontal Check
    std::string horizontal;
    int startColumn = targetCol;
    while (startColumn > 0 && game.grid[startColumn - 1][targetRow] != 0) --startColumn;
    for (int col = startColumn; col < GRID_SIZE; ++col) {
        char currentTile = (col == targetCol) ? placedLetter : game.grid[col][targetRow];
        if (currentTile == 0) break;
        if (horizontal.size() < static_cast<size_t>(MAX_WORD_LENGTH - 1)) horizontal.push_back(currentTile);
    }
    
    if (horizontal.size() > 1 && dictionary.isValidWord(horizontal) && !isWordAlreadyScored(game, horizontal)) {
        int wordScore = calculateWordScore(horizontal);
        if (dictionary.isBonusWord(horizontal)) wordScore += BONUS_WORD_POINTS;
        totalScoreGained += wordScore;
        if (game.scoredWordCount < MAX_SCORED_WORDS) {
            game.scoredWords[game.scoredWordCount++] = horizontal;
        }
    }

    // Vertical Check
    std::string vertical;
    int startRow = targetRow;
    while (startRow > 0 && game.grid[targetCol][startRow - 1] != 0) --startRow;
    for (int row = startRow; row < GRID_SIZE; ++row) {
        char currentTile = (row == targetRow) ? placedLetter : game.grid[targetCol][row];
        if (currentTile == 0) break;
        if (vertical.size() < static_cast<size_t>(MAX_WORD_LENGTH - 1)) vertical.push_back(currentTile);
    }
    
    if (vertical.size() > 1 && dictionary.isValidWord(vertical) && !isWordAlreadyScored(game, vertical)) {
        int wordScore = calculateWordScore(vertical);
        if (dictionary.isBonusWord(vertical)) wordScore += BONUS_WORD_POINTS;
        totalScoreGained += wordScore;
        if (game.scoredWordCount < MAX_SCORED_WORDS) {
            game.scoredWords[game.scoredWordCount++] = vertical;
        }
    }

    return totalScoreGained;
}