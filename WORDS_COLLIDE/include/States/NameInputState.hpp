#ifndef NAME_INPUT_STATE_HPP
#define NAME_INPUT_STATE_HPP

#include "Core/IGameState.hpp"

class NameInputState : public IGameState {
public:
    void handleInput(GameEngine& engine, const SDL_Event& event) override;
    void update(GameEngine& engine, Uint32 deltaMs) override;
    void render(GameEngine& engine) override;
};

#endif // NAME_INPUT_STATE_HPP