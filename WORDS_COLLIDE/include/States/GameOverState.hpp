#ifndef GAME_OVER_STATE_HPP
#define GAME_OVER_STATE_HPP

#include "Core/IGameState.hpp"
#include "UI/Button.hpp"
#include <memory>

class GameOverState : public IGameState {
public:
    void onEnter(GameEngine& engine) override;
    void handleInput(GameEngine& engine, const SDL_Event& event) override;
    void update(GameEngine& engine, Uint32 deltaMs) override;
    void render(GameEngine& engine) override;

private:
    std::shared_ptr<Button> playAgainBtn;
};

#endif // GAME_OVER_STATE_HPP