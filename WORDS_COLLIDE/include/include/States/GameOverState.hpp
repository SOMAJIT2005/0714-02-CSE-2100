#ifndef GAME_OVER_STATE_HPP
#define GAME_OVER_STATE_HPP

#include "Core/IGameState.hpp"

class GameOverState : public IGameState {
public:
    void handleInput(GameEngine& engine, const SDL_Event& event) override;
    void update(GameEngine& engine, Uint32 deltaMs) override;
    void render(GameEngine& engine) override;
};

#endif // GAME_OVER_STATE_HPP