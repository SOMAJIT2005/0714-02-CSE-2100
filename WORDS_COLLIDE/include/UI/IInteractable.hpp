#ifndef I_INTERACTABLE_HPP
#define I_INTERACTABLE_HPP

#include <SDL2/SDL.h>

class IInteractable {
public:
    virtual ~IInteractable() = default;
    
    // Returns true if the element consumed the event (e.g., a button was clicked)
    virtual bool handleInput(const SDL_Event& event) = 0;
};

#endif // I_INTERACTABLE_HPP