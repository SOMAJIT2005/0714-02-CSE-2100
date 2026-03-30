#include "Systems/Renderer.hpp"
#include "Core/Config.hpp"
#include <SDL2/SDL_ttf.h>

Renderer::Renderer(AppContext& ctx) : app(ctx) {}

void Renderer::clear(SDL_Color color) {
    SDL_SetRenderDrawColor(app.renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(app.renderer);
}

void Renderer::present() {
    SDL_RenderPresent(app.renderer);
}

void Renderer::drawText(TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) {
    if (text.empty() || !font) return;
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(app.renderer, surface);
    SDL_Rect rect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(app.renderer, texture, nullptr, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void Renderer::drawTextCentered(TTF_Font* font, const std::string& text, int y, SDL_Color color) {
    int w = 0, h = 0;
    TTF_SizeText(font, text.c_str(), &w, &h);
    int x = (WINDOW_WIDTH - w) / 2;
    drawText(font, text, x, y, color);
}



void Renderer::drawRect(const SDL_Rect& rect, SDL_Color color, bool fill) {
    SDL_SetRenderDrawColor(app.renderer, color.r, color.g, color.b, color.a);
    if (fill) SDL_RenderFillRect(app.renderer, &rect);
    else SDL_RenderDrawRect(app.renderer, &rect);
}

void Renderer::drawLine(int x1, int y1, int x2, int y2, SDL_Color color) {
    SDL_SetRenderDrawColor(app.renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(app.renderer, x1, y1, x2, x2);
}