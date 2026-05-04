#pragma once
#include "MVC/PlayModel.hpp"
#include "MVC/PlayView.hpp"
#include "Systems/Network.hpp"
#include "Systems/Audio.hpp"
#include "Core/Config.hpp"      // Where AppContext is defined
#include <SDL2/SDL.h>

class PlayController {
private:
    PlayModel* model;
    PlayView* view;
    Network& network;
    Audio& audio;
    AppContext& ctx;

public:
    PlayController(PlayModel* m, PlayView* v, Network& n, Audio& a, AppContext& c);

    // Returns an integer state flag (e.g., 1 if GameOver state should trigger, 0 for normal)
    int processInput(const SDL_Event& event); 
    void processNetwork();
};