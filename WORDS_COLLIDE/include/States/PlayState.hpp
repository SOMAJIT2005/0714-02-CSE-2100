#pragma once
#include "Core/IGameState.hpp"
#include "MVC/PlayModel.hpp"
#include "MVC/PlayView.hpp"
#include "MVC/PlayController.hpp"

// Note: IGameState.hpp probably already includes SDL.h and GameEngine, 
// but if the compiler complains, you may need to #include "Core/GameEngine.hpp"

class PlayState : public IGameState {
private:
    PlayModel* model;
    PlayView* view;
    PlayController* controller;

public:
    // Notice how these now exactly match the parameters your engine uses
    void onEnter(GameEngine& engine) override;
    void handleInput(GameEngine& engine, const SDL_Event& event) override;
    void update(GameEngine& engine, Uint32 deltaMs) override;
    void render(GameEngine& engine) override;
    
    // Check your IGameState.hpp to see if onExit takes (GameEngine& engine) or ()
    ~PlayState();
};