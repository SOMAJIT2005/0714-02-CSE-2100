#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "UI/IDrawable.hpp"
#include "UI/IInteractable.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class Button : public IDrawable, public IInteractable {
public:
    SDL_Rect rect;
    Button(SDL_Rect rect, const std::string& text, TTF_Font* font);

    bool handleInput(const SDL_Event& event) override;
    void render(Renderer& renderer) const override;

private:
    std::string text;
    TTF_Font* font;
    bool isHovered;
    
    SDL_Color colorNormal = {100, 50, 150, 255};
    SDL_Color colorHover = {160, 130, 200, 255};
    SDL_Color colorText = {255, 255, 255, 255};
};

#endif // BUTTON_HPP