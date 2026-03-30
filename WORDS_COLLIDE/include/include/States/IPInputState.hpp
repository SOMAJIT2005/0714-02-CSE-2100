#ifndef IP_INPUT_STATE_HPP
#define IP_INPUT_STATE_HPP

#include "Core/IGameState.hpp"

class IPInputState : public IGameState {
public:
    void handleInput(GameEngine& engine, const SDL_Event& event) override;
    void update(GameEngine& engine, Uint32 deltaMs) override;
    void render(GameEngine& engine) override;
};

#endif // IP_INPUT_STATE_HPP