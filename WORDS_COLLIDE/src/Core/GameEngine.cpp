#include "Core/GameEngine.hpp"
#include "Systems/Network.hpp"
#include "States/SplashState.hpp" // Assuming you start in the Splash state
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

// Constructor initializes the subsystems that require references
GameEngine::GameEngine() 
    : isRunning(false), 
      lastTime(0), 
      scorer(dictionary), 
      audio(app), 
      renderer(app) {
}

GameEngine::~GameEngine() {
    // Cleanup mirrors your old Core_Cleanup
    Network::cleanup();
    
    if (app.fontRegular) TTF_CloseFont(app.fontRegular);
    if (app.fontLarge) TTF_CloseFont(app.fontLarge);
    if (app.renderer) SDL_DestroyRenderer(app.renderer);
    if (app.window) SDL_DestroyWindow(app.window);
    
    TTF_Quit();
    SDL_Quit();
}

bool GameEngine::init() {
    // 1. Initialize core SDL systems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return false;
    }
    
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << "\n";
        return false;
    }

    // 2. Create Window
    app.window = SDL_CreateWindow(
        "WORDS_COLLIDE", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 
        SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS
    );
    if (!app.window) return false;

    // 3. Create Renderer
    app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_SOFTWARE);
    if (!app.renderer) return false;

    // 4. Load Assets
    app.fontRegular = TTF_OpenFont("assets/arial.ttf", 24);
    app.fontLarge = TTF_OpenFont("assets/arial.ttf", 48);

    // 5. Initialize Subsystems
    Network::init();
    if (!audio.init()) {
        std::cerr << "Warning: Audio failed to initialize. Playing silently.\n";
    }
    
    // Load Dictionary (SRP: GameEngine delegates this to Dictionary class)
    dictionary.loadFromFile("assets/Dictionary.txt");

    // 6. Setup Initial Game Data (Resetting grids, buttons, etc.)
    state.currentState = GameStateType::SPLASH;
    state.gameStartTime = 0;
    state.scores = {0, 0};
    for (auto &row : state.grid) row.fill(0);
    
    // Configure default UI button positions (Consider moving this to a UI factory later!)
    state.startGameButton.rect = { (WINDOW_WIDTH - 250) / 2, 350, 250, 50 };
    state.startGameButton.text = "Play on One Device";
    state.multiplayerButton.rect = { (WINDOW_WIDTH - 250) / 2, 430, 250, 50 };
    state.multiplayerButton.text = "Multiplayer (Wi-Fi)";
    state.hostGameButton.rect = { (WINDOW_WIDTH - 250) / 2, 330, 250, 50 };
    state.hostGameButton.text = "Host a Game";
    state.joinGameButton.rect = { (WINDOW_WIDTH - 250) / 2, 410, 250, 50 };
    state.joinGameButton.text = "Join a Game";
    state.backButton.rect = { (WINDOW_WIDTH - 250) / 2, 490, 250, 50 };
    state.backButton.text = "Back";
    state.giveUpButton.rect = { WINDOW_WIDTH - 130, WINDOW_HEIGHT - 45, 120, 35 };
    state.giveUpButton.text = "Give Up";
    state.playAgainButton.rect = { (WINDOW_WIDTH - 150) / 2, 480, 150, 40 };
    state.playAgainButton.text = "Play Again";

    // Start Engine Loop
    isRunning = true;
    lastTime = SDL_GetTicks();
    
    // OCP: Set the starting state
    changeState(std::make_unique<SplashState>());

    return true;
}

void GameEngine::changeState(std::unique_ptr<IGameState> newState) {
    if (currentState) {
        currentState->onExit(*this);
    }
    currentState = std::move(newState);
    if (currentState) {
        currentState->onEnter(*this);
    }
}

void GameEngine::run() {
    SDL_Event event;
    while (isRunning) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 deltaMs = currentTime - lastTime;
        lastTime = currentTime;

        // 1. Process Input 
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit();
            } else if (currentState) {
                // Pass input to the active state
                currentState->handleInput(*this, event);
            }
        }

        // 2. Update Logic 
        if (currentState) {
            currentState->update(*this, deltaMs);
        }

        // 3. Render
        if (currentState) {
            renderer.clear({230, 210, 255, 255}); // Default background
            currentState->render(*this);
            renderer.present();
        }

        // Optional: Keep window active (Avoid putting this directly inside the loop if possible, 
        // but left here to mirror your original behavior)
        // SDL_RaiseWindow(app.window); 

        SDL_Delay(10);
    }
}

void GameEngine::quit() {
    isRunning = false;
}