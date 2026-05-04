#pragma once
#include "Core/IGameState.hpp"

class NameInputState : public IGameState {
public:
    void onEnter(GameEngine& engine) override {}
    void handleInput(GameEngine& engine, const SDL_Event& event) override;
    void update(GameEngine& engine, Uint32 deltaMs) override;
    void render(GameEngine& engine) override;
};