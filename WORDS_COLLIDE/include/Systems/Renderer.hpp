#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Core/Types.hpp"
#include "Systems/Renderer.hpp"
#include <string>

class Renderer  {
public:
    explicit Renderer(AppContext& ctx);
    
    void clear(SDL_Color color);
    void present();
    void drawText(TTF_Font* font, const std::string& text, int x, int y, SDL_Color color);
    void drawTextCentered(TTF_Font* font, const std::string& text, int y, SDL_Color color);
    void drawRect(const SDL_Rect& rect, SDL_Color color, bool fill = false);
    void drawLine(int x1, int y1, int x2, int y2, SDL_Color color);

private:
    AppContext& app;
};

#endif // RENDERER_HPP