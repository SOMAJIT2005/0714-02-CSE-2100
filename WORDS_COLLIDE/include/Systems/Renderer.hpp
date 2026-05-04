#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class Renderer {
private:
    SDL_Renderer* renderer;

public:
    Renderer() : renderer(nullptr) {}
    ~Renderer() {}

    bool init(SDL_Window* window);
    void clear(SDL_Color color);
    void present();
    
    void drawText(TTF_Font* font, const std::string& text, int x, int y, SDL_Color color);
    void drawTextCentered(TTF_Font* font, const std::string& text, int y, SDL_Color color);
    void drawRect(const SDL_Rect& rect, SDL_Color color, bool fill = false);
    
    // Absolute coordinate line drawing
    void drawLine(int x1, int y1, int x2, int y2, SDL_Color color);
};