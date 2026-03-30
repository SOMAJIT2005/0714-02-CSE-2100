#ifndef BOARD_SCORER_HPP
#define BOARD_SCORER_HPP

#include "Game/IBoardScorer.hpp"
#include "Game/Dictionary.hpp"

class BoardScorer : public IBoardScorer {
public:
    explicit BoardScorer(const Dictionary& dict);
    int checkAndScore(GameState& game, int targetCol, int targetRow, char placedLetter) const override;

private:
    const Dictionary& dictionary;
    int calculateWordScore(const std::string& word) const;
    bool isWordAlreadyScored(const GameState& game, const std::string& word) const;
};

#endif // BOARD_SCORER_HPP