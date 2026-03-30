#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include "Core/Types.hpp"

class InputHandler {
public:
    static bool isMouseOverButton(const SDL_Event& event, const Button& button);
    static bool isMouseClickOnButton(const SDL_Event& event, const Button& button);
    static bool getGridClick(const SDL_Event& event, int& outX, int& outY);
    
    // Text Input Utilities
    static bool isBackspace(const SDL_Event& event);
    static bool isEnter(const SDL_Event& event);
    static bool isAlphaNumericInput(const SDL_Event& event, char& outChar);
};

#endif // INPUT_HANDLER_HPP