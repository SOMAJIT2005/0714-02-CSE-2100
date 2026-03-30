#ifndef SPLASH_STATE_HPP
#define SPLASH_STATE_HPP

#include "Core/IGameState.hpp"
#include "UI/Button.hpp"
#include "UI/Label.hpp"
#include <vector>
#include <memory>

class SplashState : public IGameState {
public:
    void onEnter(GameEngine& engine) override;
    void handleInput(GameEngine& engine, const SDL_Event& event) override;
    void update(GameEngine& engine, Uint32 deltaMs) override;
    void render(GameEngine& engine) override;

private:
    // LSP in action: We can store pointers to the Interfaces!
    std::vector<std::shared_ptr<IDrawable>> drawables;
    std::shared_ptr<Button> startBtn;
    std::shared_ptr<Button> multiBtn;
};

#endif // SPLASH_STATE_HPP