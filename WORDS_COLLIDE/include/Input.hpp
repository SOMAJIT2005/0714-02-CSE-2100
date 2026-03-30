#ifndef INPUT_HPP
#define INPUT_HPP

#include "Types.hpp"

class Input {
public:
    static bool isMouseOverButton(const SDL_Event& event, const Button& button);
    static void handleSplash(const SDL_Event& event, GameState& game);
    static void handleMultiplayerMenu(const SDL_Event& event, GameState& game);
    static void handleIPInput(const SDL_Event& event, GameState& game);
    static void handleNames(const SDL_Event& event, GameState& game, AppContext& app);
    static void handleGame(const SDL_Event& event, GameState& game, AppContext& app);
    static void handleGameOver(const SDL_Event& event, GameState& game);
};

#endif // INPUT_HPP
