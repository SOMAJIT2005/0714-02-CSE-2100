#ifndef I_RENDERER_HPP
#define I_RENDERER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void clear(SDL_Color color) = 0;
    virtual void present() = 0;
    virtual void drawText(TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) = 0;
    virtual void drawTextCentered(TTF_Font* font, const std::string& text, int y, SDL_Color color) = 0;
    virtual void drawRect(const SDL_Rect& rect, SDL_Color color, bool fill = false) = 0;
    virtual void drawLine(int x1, int y1, int x2, int y2, SDL_Color color) = 0;
};

#endif // I_RENDERER_HPP