#ifndef IGAMESTATE_HPP
#define IGAMESTATE_HPP

#include <SDL2/SDL.h>

class GameEngine; // Forward declaration

class IGameState {
public:
    virtual ~IGameState() = default;
    
    virtual void onEnter(GameEngine& engine) {}
    virtual void onExit(GameEngine& engine) {}
    
    virtual void handleInput(GameEngine& engine, const SDL_Event& event) = 0;
    virtual void update(GameEngine& engine, Uint32 deltaMs) = 0;
    virtual void render(GameEngine& engine) = 0;
};

#endif // IGAMESTATE_HPP