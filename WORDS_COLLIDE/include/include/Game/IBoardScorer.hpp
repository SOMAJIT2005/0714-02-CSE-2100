#ifndef I_BOARD_SCORER_HPP
#define I_BOARD_SCORER_HPP

#include "Core/Types.hpp"

class IBoardScorer {
public:
    virtual ~IBoardScorer() = default;
    virtual int checkAndScore(GameState& game, int targetCol, int targetRow, char placedLetter) const = 0;
};

#endif // I_BOARD_SCORER_HPP