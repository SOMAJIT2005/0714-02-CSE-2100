#include "MVC/PlayController.hpp"
#include "Systems/InputHandler.hpp"
#include <cctype>

PlayController::PlayController(PlayModel* m, PlayView* v, Network& n, Audio& a, AppContext& c)
    : model(m), view(v), network(n), audio(a), ctx(c) {}

int PlayController::processInput(const SDL_Event& event) {
    GameState& data = model->getData();

    if (view->getGiveUpButton() && view->getGiveUpButton()->handleInput(event)) {
        audio.playSound(ctx.sfxWin);
        return 1; 
    }

    int clickX, clickY;
    if (InputHandler::getGridClick(event, clickX, clickY)) {
        if (data.grid[clickX][clickY] == 0) {
            data.isTileSelected = true;
            data.selectedX = clickX;
            data.selectedY = clickY;
            SDL_StartTextInput();
        }
    }

    char c;
    if (data.isTileSelected && InputHandler::isAlphaNumericInput(event, c)) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            data.currentLetter = std::toupper(c);
        }
    }

    if (data.isTileSelected && InputHandler::isEnter(event) && data.currentLetter) {
        char letterToSend = data.currentLetter;
        data.grid[data.selectedX][data.selectedY] = data.currentLetter;
        
        audio.playSound(ctx.sfxPlaceTile);

        int score = model->getScorer().checkAndScore(data, data.selectedX, data.selectedY, data.currentLetter);
        if (score > 0) {
            audio.playSound(ctx.sfxValidWord);
            data.scores[data.currentPlayer] += score;
        }

        network.sendMove(data.selectedX, data.selectedY, letterToSend);

        data.currentPlayer = (data.currentPlayer + 1) % 2;
        data.isTileSelected = false;
        data.currentLetter = '\0';
        data.turnStartTime = SDL_GetTicks();
        SDL_StopTextInput();
    }
    
    return 0; 
}

void PlayController::processNetwork() {
    NetworkMove enemyMove;
    GameState& data = model->getData();
    
    if (network.receiveMove(enemyMove)) {
        audio.playSound(ctx.sfxPlaceTile);
        data.grid[enemyMove.x][enemyMove.y] = enemyMove.letter;
        
        int score = model->getScorer().checkAndScore(data, enemyMove.x, enemyMove.y, enemyMove.letter);
        if (score > 0) {
            audio.playSound(ctx.sfxValidWord);
            data.scores[data.currentPlayer] += score;
        }
        
        data.currentPlayer = (data.currentPlayer + 1) % 2;
        data.isTileSelected = false;
        data.currentLetter = '\0';
        data.turnStartTime = SDL_GetTicks();
        SDL_StopTextInput();
    }
}