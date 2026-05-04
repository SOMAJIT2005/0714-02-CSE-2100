#include "MVC/PlayModel.hpp"
#include <SDL2/SDL.h>

PlayModel::PlayModel(GameState& state, IBoardScorer& boardScorer)
    : data(state), scorer(boardScorer) {}

void PlayModel::updateTimers(Uint32 currentTime) {
    if (currentTime - data.turnStartTime >= TURN_DURATION_MS) {
        data.currentPlayer = (data.currentPlayer + 1) % 2;
        data.isTileSelected = false;
        data.currentLetter = '\0';
        data.turnStartTime = SDL_GetTicks();
        SDL_StopTextInput();
    }
}

bool PlayModel::checkTimeUp(Uint32 currentTime) const {
    Uint32 elapsedMs = currentTime - data.gameStartTime;
    return elapsedMs >= GAME_DURATION_MS;
}

GameState& PlayModel::getData() const { 
    return data; 
}

IBoardScorer& PlayModel::getScorer() const { 
    return scorer; 
}