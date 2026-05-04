#include "Core/GameEngine.hpp"
#include "Systems/Audio.hpp"
#include "Systems/Renderer.hpp"
#include "Systems/Network.hpp"
#include "Game/BoardScorer.hpp"
#include "Game/Dictionary.hpp"
#include "States/SplashState.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    // 1. Core SDL Bootstrapping
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 || TTF_Init() == -1) return 1;

    AppContext app{};
    app.window = SDL_CreateWindow("WORDS_COLLIDE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_SHOWN);
    app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_SOFTWARE);
    app.fontRegular = TTF_OpenFont("assets/arial.ttf", 24);
    app.fontLarge = TTF_OpenFont("assets/arial.ttf", 48);

    // 2. Instantiate Rules
    Dictionary dict;
    dict.loadFromFile("assets/Dictionary.txt");

    // 3. Dependency Injection! Create the concrete systems...
    auto audio = std::make_unique<Audio>(app);
    auto renderer = std::make_unique<Renderer>();
    auto network = std::make_unique<Network>();
    auto scorer = std::make_unique<BoardScorer>(dict);

    // ...and inject them into the high-level policy engine.
    GameEngine engine(app, std::move(audio), std::move(renderer), std::move(network), std::move(scorer));

    // 4. Run
    // Set the starting screen, then run the game!
   engine.changeState(std::make_unique<SplashState>());
   engine.run();

    // Cleanup
    if (app.fontRegular) TTF_CloseFont(app.fontRegular);
    if (app.fontLarge) TTF_CloseFont(app.fontLarge);
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}