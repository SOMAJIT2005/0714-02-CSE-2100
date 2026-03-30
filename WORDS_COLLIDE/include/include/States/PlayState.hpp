#ifndef PLAY_STATE_HPP
#define PLAY_STATE_HPP

#include "Core/IGameState.hpp"

class PlayState : public IGameState {
public:
    void onEnter(GameEngine& engine) override;
    void handleInput(GameEngine& engine, const SDL_Event& event) override;
    void update(GameEngine& engine, Uint32 deltaMs) override;
    void render(GameEngine& engine) override;

private:
    void processNetworkMove(GameEngine& engine);
};

#endif // PLAY_STATE_HPP