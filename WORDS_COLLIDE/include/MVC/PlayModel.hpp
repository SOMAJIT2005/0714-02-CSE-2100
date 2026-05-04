#pragma once
#include "Core/Types.hpp"       // Where GameState is defined
#include "Game/IBoardScorer.hpp"

class PlayModel {
private:
    GameState& data;            // Reference to your existing game data
    IBoardScorer& scorer;       // Interface for scoring

public:
    PlayModel(GameState& state, IBoardScorer& boardScorer);

    // Logic methods
    void updateTimers(Uint32 currentTime);
    bool checkTimeUp(Uint32 currentTime) const;
    
    // We expose the data so the View can read it
    GameState& getData() const; 
    IBoardScorer& getScorer() const;
};