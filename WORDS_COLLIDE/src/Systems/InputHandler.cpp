#include "Systems/InputHandler.hpp"
#include <cctype>

bool InputHandler::isMouseOverButton(const SDL_Event& event, const Button& button) {
    if (event.type == SDL_MOUSEMOTION) {
        int x = event.motion.x;
        int y = event.motion.y;
        return (x >= button.rect.x && x <= button.rect.x + button.rect.w &&
                y >= button.rect.y && y <= button.rect.y + button.rect.h);
    }
    return false;
}

bool InputHandler::isMouseClickOnButton(const SDL_Event& event, const Button& button) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x = event.button.x;
        int y = event.button.y;
        return (x >= button.rect.x && x <= button.rect.x + button.rect.w &&
                y >= button.rect.y && y <= button.rect.y + button.rect.h);
    }
    return false;
}

bool InputHandler::getGridClick(const SDL_Event& event, int& outX, int& outY) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        outX = event.button.x / TILE_SIZE;
        outY = event.button.y / TILE_SIZE;
        return (outX >= 0 && outX < GRID_SIZE && outY >= 0 && outY < GRID_SIZE);
    }
    return false;
}

bool InputHandler::isBackspace(const SDL_Event& event) {
    return (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE);
}

bool InputHandler::isEnter(const SDL_Event& event) {
    return (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN);
}

bool InputHandler::isAlphaNumericInput(const SDL_Event& event, char& outChar) {
    if (event.type == SDL_TEXTINPUT) {
        char c = event.text.text[0];
        if (std::isalnum(static_cast<unsigned char>(c)) || std::isspace(static_cast<unsigned char>(c)) || c == '.') {
            outChar = c;
            return true;
        }
    }
    return false;
}