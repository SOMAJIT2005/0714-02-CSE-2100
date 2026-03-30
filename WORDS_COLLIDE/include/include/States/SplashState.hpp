#ifndef SPLASH_STATE_HPP
#define SPLASH_STATE_HPP

#include "Core/IGameState.hpp"

class SplashState : public IGameState {
public:
    void onEnter(GameEngine& engine) override;
    void handleInput(GameEngine& engine, const SDL_Event& event) override;
    void update(GameEngine& engine, Uint32 deltaMs) override;
    void render(GameEngine& engine) override;
};

#endif // SPLASH_STATE_HPP