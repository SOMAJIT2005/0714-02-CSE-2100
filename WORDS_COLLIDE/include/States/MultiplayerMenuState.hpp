#ifndef MULTIPLAYER_MENU_STATE_HPP
#define MULTIPLAYER_MENU_STATE_HPP

#include "Core/IGameState.hpp"
#include "UI/Button.hpp"
#include "UI/Label.hpp"
#include <vector>
#include <memory>

class MultiplayerMenuState : public IGameState {
public:
    void onEnter(GameEngine& engine) override;
    void handleInput(GameEngine& engine, const SDL_Event& event) override;
    void update(GameEngine& engine, Uint32 deltaMs) override;
    void render(GameEngine& engine) override;

private:
    std::vector<std::shared_ptr<IDrawable>> drawables;
    std::shared_ptr<Button> hostBtn;
    std::shared_ptr<Button> joinBtn;
    std::shared_ptr<Button> backBtn;
};

#endif // MULTIPLAYER_MENU_STATE_HPP