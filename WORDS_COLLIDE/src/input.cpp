#include "Input.hpp"
#include "Network.hpp"
#include "Audio.hpp"
#include "Logic.hpp"
#include <SDL2/SDL.h>
#include <cstring>
#include <cctype>

bool Input::isMouseOverButton(const SDL_Event& event, const Button& button) {
    if (event.type == SDL_MOUSEMOTION) {
        int x = event.motion.x;
        int y = event.motion.y;
        return (x >= button.rect.x && x <= button.rect.x + button.rect.w &&
                y >= button.rect.y && y <= button.rect.y + button.rect.h);
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x = event.button.x;
        int y = event.button.y;
        return (x >= button.rect.x && x <= button.rect.x + button.rect.w &&
                y >= button.rect.y && y <= button.rect.y + button.rect.h);
    }
    return false;
}

void Input::handleSplash(const SDL_Event& event, GameState& game) {
    if (event.type == SDL_QUIT) game.currentState = GameStateType::QUIT;
    else if (event.type == SDL_MOUSEMOTION) {
        game.startGameButton.isHovered = isMouseOverButton(event, game.startGameButton);
        game.multiplayerButton.isHovered = isMouseOverButton(event, game.multiplayerButton);
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (isMouseOverButton(event, game.startGameButton)) {
            game.currentState = GameStateType::GET_NAMES;
            SDL_StartTextInput();
        } else if (isMouseOverButton(event, game.multiplayerButton)) {
            game.currentState = GameStateType::MULTIPLAYER_MENU;
        }
    }
}

void Input::handleMultiplayerMenu(const SDL_Event& event, GameState& game) {
    if (event.type == SDL_QUIT) game.currentState = GameStateType::QUIT;
    else if (event.type == SDL_MOUSEMOTION) {
        game.hostGameButton.isHovered = isMouseOverButton(event, game.hostGameButton);
        game.joinGameButton.isHovered = isMouseOverButton(event, game.joinGameButton);
        game.backButton.isHovered = isMouseOverButton(event, game.backButton);
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (isMouseOverButton(event, game.hostGameButton)) {
            if (Network::hostGame(8080)) {
                game.currentState = GameStateType::GET_NAMES;
                SDL_StartTextInput();
            }
        } else if (isMouseOverButton(event, game.joinGameButton)) {
            game.targetIP.clear();
            game.currentState = GameStateType::ENTER_IP;
            SDL_StartTextInput();
        } else if (isMouseOverButton(event, game.backButton)) {
            game.currentState = GameStateType::SPLASH;
        }
    }
}

void Input::handleIPInput(const SDL_Event& event, GameState& game) {
    if (event.type == SDL_QUIT) game.currentState = GameStateType::QUIT;
    else if (event.type == SDL_MOUSEMOTION) {
        game.backButton.isHovered = isMouseOverButton(event, game.backButton);
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (isMouseOverButton(event, game.backButton)) {
            game.currentState = GameStateType::MULTIPLAYER_MENU;
            SDL_StopTextInput();
        }
    } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_BACKSPACE) {
            if (!game.targetIP.empty()) game.targetIP.pop_back();
        } else if (event.key.keysym.sym == SDLK_RETURN) {
            if (!game.targetIP.empty()) {
                if (Network::joinGame(game.targetIP.c_str(), 8080)) {
                    game.currentState = GameStateType::GET_NAMES;
                } else {
                    game.targetIP.clear();
                }
            }
        }
    } else if (event.type == SDL_TEXTINPUT) {
        if (game.targetIP.size() < 15) {
            char c = event.text.text[0];
            if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') game.targetIP += event.text.text;
        }
    }
}

void Input::handleNames(const SDL_Event& event, GameState& game, AppContext& app) {
    if (event.type == SDL_QUIT) game.currentState = GameStateType::QUIT;
    else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_BACKSPACE) {
            auto &s = game.playerNames[game.currentNameInput];
            if (!s.empty()) s.pop_back();
        } else if (event.key.keysym.sym == SDLK_RETURN) {
            if (!game.playerNames[game.currentNameInput].empty()) {
                if (game.currentNameInput == 0) game.currentNameInput = 1;
                else {
                    game.currentState = GameStateType::PLAYING;
                    game.gameStartTime = SDL_GetTicks();
                    game.turnStartTime = SDL_GetTicks();
                    SDL_StopTextInput();
                    // Audio stop handled by caller (main)
                }
            }
        }
    } else if (event.type == SDL_TEXTINPUT) {
        auto &s = game.playerNames[game.currentNameInput];
        if (s.size() < MAX_NAME_LENGTH - 1) {
            char c = event.text.text[0];
            if (std::isalnum(static_cast<unsigned char>(c)) || std::isspace(static_cast<unsigned char>(c))) s += event.text.text;
        }
    }
}

void Input::handleGame(const SDL_Event& event, GameState& game, AppContext& app) {
    if (event.type == SDL_QUIT) game.currentState = GameStateType::QUIT;
    else if (event.type == SDL_MOUSEMOTION) {
        game.giveUpButton.isHovered = isMouseOverButton(event, game.giveUpButton);
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (isMouseOverButton(event, game.giveUpButton)) {
            Mix_HaltMusic();
            if (app.sfxWin) Mix_PlayChannel(-1, app.sfxWin, 0);
            game.currentState = GameStateType::GAME_OVER;
            SDL_StopTextInput();
            return;
        }
        int x = event.button.x / TILE_SIZE;
        int y = event.button.y / TILE_SIZE;
        if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE && game.grid[x][y] == 0) {
            game.isTileSelected = true;
            game.selectedX = x;
            game.selectedY = y;
            SDL_StartTextInput();
        }
    } else if (event.type == SDL_KEYDOWN) {
        if (game.isTileSelected && event.key.keysym.sym == SDLK_RETURN && game.currentLetter) {
            char letterToSend = game.currentLetter;
            game.grid[game.selectedX][game.selectedY] = game.currentLetter;
            if (app.sfxPlaceTile) Mix_PlayChannel(-1, app.sfxPlaceTile, 0);

            int score = Logic::checkAndScore(game, game.selectedX, game.selectedY, game.currentLetter);
            if (score > 0) {
                if (app.sfxValidWord) Mix_PlayChannel(-1, app.sfxValidWord, 0);
                game.scores[game.currentPlayer] += score;
            }

            Network::sendMove(game.selectedX, game.selectedY, letterToSend);

            game.currentPlayer = (game.currentPlayer + 1) % 2;
            game.isTileSelected = false;
            game.currentLetter = '\0';
            game.turnStartTime = SDL_GetTicks();
            SDL_StopTextInput();
        }
    } else if (event.type == SDL_TEXTINPUT) {
        if (game.isTileSelected && std::strlen(event.text.text) == 1 && std::isalpha(static_cast<unsigned char>(event.text.text[0]))) {
            game.currentLetter = std::toupper(static_cast<unsigned char>(event.text.text[0]));
        }
    }
}

void Input::handleGameOver(const SDL_Event& event, GameState& game) {
    if (event.type == SDL_QUIT) game.currentState = GameStateType::QUIT;
    else if (event.type == SDL_MOUSEMOTION) {
        game.playAgainButton.isHovered = isMouseOverButton(event, game.playAgainButton);
    } else if (event.type == SDL_MOUSEBUTTONDOWN && isMouseOverButton(event, game.playAgainButton)) {
        game.currentState = GameStateType::RESTART;
    } else if (event.type == SDL_KEYDOWN) {
        game.currentState = GameStateType::QUIT;
    }
}
