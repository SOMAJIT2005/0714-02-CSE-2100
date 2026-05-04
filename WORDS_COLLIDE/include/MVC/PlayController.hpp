#pragma once
#include "MVC/PlayModel.hpp"
#include "MVC/PlayView.hpp"
#include "Systems/Network.hpp"
#include "Systems/Audio.hpp"
#include "Core/Config.hpp"
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

    int processInput(const SDL_Event& event); 
    void processNetwork();
};