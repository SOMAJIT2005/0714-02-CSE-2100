#ifndef MULTIPLAYER_MENU_STATE_HPP
#define MULTIPLAYER_MENU_STATE_HPP

#include "Core/IGameState.hpp"

class MultiplayerMenuState : public IGameState {
public:
    void handleInput(GameEngine& engine, const SDL_Event& event) override;
    void update(GameEngine& engine, Uint32 deltaMs) override;
    void render(GameEngine& engine) override;
};

#endif // MULTIPLAYER_MENU_STATE_HPP