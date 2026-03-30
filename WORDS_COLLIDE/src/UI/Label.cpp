#include "UI/Label.hpp"
#include "Systems/Renderer.hpp"

Label::Label(const std::string& text, int x, int y, TTF_Font* font, SDL_Color color)
    : text(text), x(x), y(y), font(font), color(color) {}

void Label::render(Renderer& renderer) const {
    if (font && !text.empty()) {
        renderer.drawText(font, text, x, y, color);
    }
}

void Label::setText(const std::string& newText) {
    text = newText;
}