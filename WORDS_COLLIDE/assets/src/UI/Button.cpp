#include "UI/Button.hpp"
#include "Systems/Renderer.hpp"
#include "Systems/InputHandler.hpp"

Button::Button(SDL_Rect rect, const std::string& text, TTF_Font* font)
    : rect(rect), text(text), font(font), isHovered(false) {}

bool Button::handleInput(const SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        int x = event.motion.x;
        int y = event.motion.y;
        isHovered = (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);
        return false;
    } 
    else if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x = event.button.x;
        int y = event.button.y;
        if (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h) {
            return true; // The button was successfully clicked!
        }
    }
    return false;
}

void Button::render(Renderer& renderer) const {
    SDL_Color useColor = isHovered ? colorHover : colorNormal;
    renderer.drawRect(rect, useColor, true);

    if (font && !text.empty()) {
        int textW = 0, textH = 0;
        TTF_SizeText(font, text.c_str(), &textW, &textH);
        int textX = rect.x + (rect.w - textW) / 2;
        int textY = rect.y + (rect.h - textH) / 2;
        renderer.drawText(font, text, textX, textY, colorText);
    }
}