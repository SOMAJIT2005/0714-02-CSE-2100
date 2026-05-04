#include "States/PlayState.hpp"
#include "States/GameOverState.hpp"
#include "Core/GameEngine.hpp"
#include "Systems/InputHandler.hpp"
#include <cctype>

void PlayState::onEnter(GameEngine& engine) {
    GameState& data = engine.getGameData();
    data.gameStartTime = SDL_GetTicks();
    data.turnStartTime = SDL_GetTicks();
    engine.getAudio().stopMusic();

    giveUpBtn = std::make_shared<Button>(SDL_Rect{WINDOW_WIDTH - 130, WINDOW_HEIGHT - 45, 120, 35}, "Give Up", engine.getContext().fontRegular);
}

void PlayState::handleInput(GameEngine& engine, const SDL_Event& event) {
    GameState& data = engine.getGameData();
    IAudio& audio = engine.getAudio();

    if (giveUpBtn->handleInput(event)) {
        audio.playSound(engine.getContext().sfxWin);
        engine.changeState(std::make_unique<GameOverState>());
        return;
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
        
        audio.playSound(engine.getContext().sfxPlaceTile);

        // DIP: Using the injected Scorer interface!
        int score = engine.getScorer().checkAndScore(data, data.selectedX, data.selectedY, data.currentLetter);
        if (score > 0) {
            audio.playSound(engine.getContext().sfxValidWord);
            data.scores[data.currentPlayer] += score;
        }

        // DIP: Using the injected Network interface!
        engine.getNetwork().sendMove(data.selectedX, data.selectedY, letterToSend);

        data.currentPlayer = (data.currentPlayer + 1) % 2;
        data.isTileSelected = false;
        data.currentLetter = '\0';
        data.turnStartTime = SDL_GetTicks();
        SDL_StopTextInput();
    }
}

void PlayState::update(GameEngine& engine, Uint32 deltaMs) {
    GameState& data = engine.getGameData();
    IAudio& audio = engine.getAudio();
    Uint32 currentTime = SDL_GetTicks();
    Uint32 elapsedMs = currentTime - data.gameStartTime;

    if (elapsedMs >= GAME_DURATION_MS) {
        audio.playSound(engine.getContext().sfxWin);
        engine.changeState(std::make_unique<GameOverState>());
        return;
    }

    Uint32 remainingMs = GAME_DURATION_MS > elapsedMs ? GAME_DURATION_MS - elapsedMs : 0;
    if (remainingMs < WARNING_TIME_MS && currentTime - data.lastWarningTime > WARNING_INTERVAL_MS) {
        audio.playSound(engine.getContext().sfxWarning);
        data.lastWarningTime = currentTime;
    }

    if (currentTime - data.turnStartTime >= TURN_DURATION_MS) {
        data.currentPlayer = (data.currentPlayer + 1) % 2;
        data.isTileSelected = false;
        data.currentLetter = '\0';
        data.turnStartTime = SDL_GetTicks();
        SDL_StopTextInput();
    }

    processNetworkMove(engine);
}

void PlayState::processNetworkMove(GameEngine& engine) {
    NetworkMove enemyMove;
    
    // DIP: Using the injected Network interface!
    if (engine.getNetwork().receiveMove(enemyMove)) {
        GameState& data = engine.getGameData();
        IAudio& audio = engine.getAudio();

        audio.playSound(engine.getContext().sfxPlaceTile);
        data.grid[enemyMove.x][enemyMove.y] = enemyMove.letter;
        
        int score = engine.getScorer().checkAndScore(data, enemyMove.x, enemyMove.y, enemyMove.letter);
        if (score > 0) {
            audio.playSound(engine.getContext().sfxValidWord);
            data.scores[data.currentPlayer] += score;
        }
        
        data.currentPlayer = (data.currentPlayer + 1) % 2;
        data.isTileSelected = false;
        data.currentLetter = '\0';
        data.turnStartTime = SDL_GetTicks();
        SDL_StopTextInput();
    }
}

void PlayState::render(GameEngine& engine) {
    IRenderer& renderer = engine.getRenderer();
    AppContext& ctx = engine.getContext();
    GameState& data = engine.getGameData();

    char timerText[32];
    Uint32 remaining = GAME_DURATION_MS > (SDL_GetTicks() - data.gameStartTime) ? GAME_DURATION_MS - (SDL_GetTicks() - data.gameStartTime) : 0;
    snprintf(timerText, sizeof(timerText), "Time: %02d:%02d", (remaining / 1000) / 60, (remaining / 1000) % 60);
    renderer.drawText(ctx.fontRegular, timerText, WINDOW_WIDTH - 150, 10, {100, 50, 150, 255});

    for (int i = 0; i <= GRID_SIZE; ++i) {
        renderer.drawLine(i * TILE_SIZE, 0, i * TILE_SIZE, WINDOW_HEIGHT, {160, 130, 200, 255});
        renderer.drawLine(0, i * TILE_SIZE, WINDOW_WIDTH, i * TILE_SIZE, {160, 130, 200, 255});
    }

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            char c = data.grid[i][j];
            if (c) {
                char letter[2] = { c, '\0' };
                renderer.drawText(ctx.fontRegular, letter, i * TILE_SIZE + 10, j * TILE_SIZE + 10, {0, 0, 0, 255});
            }
        }
    }

    if (data.isTileSelected) {
        SDL_Rect rect = { data.selectedX * TILE_SIZE, data.selectedY * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        renderer.drawRect(rect, {150, 150, 150, 100}, true);
        if (data.currentLetter) {
            char letter[2] = { data.currentLetter, '\0' };
            renderer.drawText(ctx.fontRegular, letter, data.selectedX * TILE_SIZE + 10, data.selectedY * TILE_SIZE + 10, {0, 0, 0, 255});
        }
    }

    char scoreText[256];
    snprintf(scoreText, sizeof(scoreText), "P1: %d | P2: %d", data.scores[0], data.scores[1]);
    renderer.drawText(ctx.fontRegular, scoreText, 10, WINDOW_HEIGHT - 40, {0, 0, 0, 255});

    giveUpBtn->render(renderer);
}