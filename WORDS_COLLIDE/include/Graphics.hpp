#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "Types.hpp"
#include <string>

class Graphics {
public:
    Graphics(AppContext& ctx);
    void drawText(TTF_Font* font, const std::string& text, int x, int y, SDL_Color color);
    void drawTextCentered(TTF_Font* font, const std::string& text, int y, SDL_Color color);
    void drawButton(Button& button);
    void renderSplash(GameState& game);
    void renderMultiplayerMenu(GameState& game);
    void renderIPInput(GameState& game);
    void renderNameInput(GameState& game);
    void renderGame(GameState& game);
    void renderGameOver(GameState& game);

private:
    AppContext& app;
};

#endif // GRAPHICS_HPP
