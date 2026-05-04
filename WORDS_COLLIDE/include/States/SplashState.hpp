#pragma once
#include "Core/IGameState.hpp"
#include "UI/Button.hpp"
#include "UI/Label.hpp"
#include <vector>
#include <memory>

class SplashState : public IGameState {
private:
    std::vector<std::shared_ptr<IDrawable>> drawables;
    std::shared_ptr<Button> startBtn;
    std::shared_ptr<Button> multiBtn;

public:
    void onEnter(GameEngine& engine) override;
    void handleInput(GameEngine& engine, const SDL_Event& event) override;
    void update(GameEngine& engine, Uint32 deltaMs) override;
    void render(GameEngine& engine) override;
};