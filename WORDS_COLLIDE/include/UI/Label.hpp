#ifndef LABEL_HPP
#define LABEL_HPP

#include "UI/IDrawable.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class Label : public IDrawable {
public:
    Label(const std::string& text, int x, int y, TTF_Font* font, SDL_Color color);
    
    void render(Renderer& renderer) const override;
    void setText(const std::string& newText);

private:
    std::string text;
    int x, y;
    TTF_Font* font;
    SDL_Color color;
};

#endif // LABEL_HPP