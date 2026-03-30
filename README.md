# 🧩 WORDS_COLLIDE

> A real-time **multiplayer word board game** built with **C++ and SDL2**, featuring clean architecture through SOLID design principles, TCP-based networked gameplay, a full UI state machine, and rich audio-visual feedback.

---

## 📌 Table of Contents

1. [Overview](#overview)
2. [Features](#features)
3. [Tech Stack](#tech-stack)
4. [Project Structure](#project-structure)
5. [UML — Architecture Overview](#uml--architecture-overview)
6. [SOLID Principles Applied](#solid-principles-applied)
7. [State Machine — Game Flow](#state-machine--game-flow)
8. [Dependencies](#dependencies)
9. [Build Instructions](#build-instructions)
10. [Gameplay Guide](#gameplay-guide)
11. [Assets](#assets)

---

## 🎯 Overview

**WORDS_COLLIDE** is a competitive real-time word-placement game where two players connect over a local network, take turns placing lettered tiles on a shared board, and race to form valid words and outscore their opponent. The project is architected with a strict application of **SOLID principles**, using interface-based abstractions, a clean state machine, and SDL2 as the low-level platform layer.

---

## ✨ Features

- 🌐 **Real-time multiplayer** over TCP (host/join model via SDL_net)
- 🎮 **Full game state machine** — splash, menus, IP input, naming, gameplay, game over
- 🔤 **Dictionary validation** — words checked against a curated word corpus
- 🧮 **Grid-based scoring** — Scrabble-style tile math via `BoardScorer`
- 🔊 **Audio feedback** — distinct sounds for tile placement, success, invalid moves, and victory
- 🎨 **SDL2 rendering** — smooth UI with TrueType fonts, buttons, and labels
- 🏗️ **SOLID architecture** — every module follows a single, well-defined responsibility
- 🔌 **Dependency injection** — all SDL systems injected via abstract interfaces; easily mockable for testing

---

## 🛠️ Tech Stack

| Layer | Technology |
|---|---|
| Language | C++17 |
| Windowing & Rendering | SDL2 |
| Audio | SDL2_mixer |
| Networking | SDL2_net (TCP) |
| Fonts | SDL2_ttf |
| Build System | CMake |
| Architecture Pattern | State Machine + Dependency Injection |

---

## 📁 Project Structure

```
WORDS_COLLIDE/
├── assets/
│   ├── Dictionary.txt               # Word validation corpus
│   ├── arial.ttf                    # Primary font
│   ├── place.wav                    # Tile placement SFX
│   ├── success.wav                  # Valid word SFX
│   ├── warning.wav                  # Invalid move SFX
│   ├── win.wav                      # Victory SFX
│   └── welcome.mp3                  # Background / welcome music
│
├── include/
│   ├── Core/
│   │   ├── Config.hpp               # Global constants & configuration
│   │   ├── Types.hpp                # Pure game data: grid, scores, IPs
│   │   ├── IGameState.hpp           # OCP: Interface for the State Pattern
│   │   └── GameEngine.hpp           # High-level policy manager
│   │
│   ├── Game/
│   │   ├── IBoardScorer.hpp         # DIP: Scoring abstraction
│   │   ├── BoardScorer.hpp          # SRP: Implements grid math
│   │   └── Dictionary.hpp           # SRP: Word validation logic
│   │
│   ├── Systems/
│   │   ├── IAudio.hpp               # DIP: Audio abstraction
│   │   ├── IRenderer.hpp            # DIP: Rendering abstraction
│   │   ├── INetwork.hpp             # DIP: Networking abstraction
│   │   ├── Audio.hpp                # SDL_mixer wrapper
│   │   ├── Renderer.hpp             # SDL drawing wrapper
│   │   ├── Network.hpp              # SDL_net TCP wrapper
│   │   └── InputHandler.hpp         # SDL event polling
│   │
│   ├── States/
│   │   ├── SplashState.hpp          # Main menu screen
│   │   ├── MultiplayerMenuState.hpp # Host / Join selection
│   │   ├── IPInputState.hpp         # Join IP typing screen
│   │   ├── NameInputState.hpp       # Player naming screen
│   │   ├── PlayState.hpp            # Core gameplay loop
│   │   └── GameOverState.hpp        # Win / Tie screen
│   │
│   └── UI/
│       ├── IDrawable.hpp            # ISP: Can be drawn
│       ├── IInteractable.hpp        # ISP: Takes input
│       ├── Button.hpp               # LSP: Substitutes both interfaces
│       └── Label.hpp                # LSP: Substitutes only IDrawable
│
└── src/
    ├── main.cpp                     # DIP: Bootstraps SDL & injects dependencies
    │
    ├── Core/
    │   └── GameEngine.cpp
    │
    ├── Game/
    │   ├── BoardScorer.cpp
    │   └── Dictionary.cpp
    │
    ├── Systems/
    │   ├── Audio.cpp
    │   ├── Renderer.cpp
    │   ├── Network.cpp
    │   └── InputHandler.cpp
    │
    ├── States/
    │   ├── SplashState.cpp
    │   ├── MultiplayerMenuState.cpp
    │   ├── IPInputState.cpp
    │   ├── NameInputState.cpp
    │   ├── PlayState.cpp
    │   └── GameOverState.cpp
    │
    └── UI/
        ├── Button.cpp
        └── Label.cpp
```

---

## 🏛️ UML — Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                          main.cpp                               │
│       (Bootstraps SDL, constructs & injects all dependencies)   │
└──────────────────────────┬──────────────────────────────────────┘
                           │ owns
                           ▼
┌─────────────────────────────────────────────────────────────────┐
│                        GameEngine                               │
│  - Holds current IGameState*                                    │
│  - Drives the main loop: handleInput / update / render          │
│  - Delegates to IAudio, IRenderer, INetwork                     │
└────────┬───────────────────────────────┬────────────────────────┘
         │ state stack                   │ system interfaces
         ▼                               ▼
┌─────────────────┐         ┌────────────────────────────────────┐
│  «interface»    │         │  «interface»    «interface»         │
│  IGameState     │         │  IAudio         IRenderer           │
│─────────────────│         │  INetwork                           │
│ + handleInput() │         └───────────────┬────────────────────┘
│ + update()      │                         │ implemented by
│ + render()      │                         ▼
│ + onEnter()     │        ┌────────────────────────────────────┐
│ + onExit()      │        │  Audio    Renderer    Network      │
└────────┬────────┘        │  (SDL_mixer / SDL2 / SDL_net)      │
         │ implemented by  └────────────────────────────────────┘
         ▼
┌───────────────────────────────────────────────────────────────┐
│                     Concrete States                           │
│  SplashState          │  MultiplayerMenuState                 │
│  IPInputState         │  NameInputState                       │
│  PlayState            │  GameOverState                        │
└──────────────────────┬────────────────────────────────────────┘
                       │ uses
                       ▼
┌───────────────────────────────────────────────────────────────┐
│                       UI Layer                                │
│                                                               │
│   «interface»              «interface»                        │
│   IDrawable                IInteractable                      │
│   + draw()                 + handleClick()                    │
│       ▲                         ▲                             │
│       │                         │                             │
│     Label ──────────────── Button                             │
│  (draw only)           (draw + click)                         │
└───────────────────────────────────────────────────────────────┘
                       │
                       ▼
┌───────────────────────────────────────────────────────────────┐
│                      Game Layer                               │
│                                                               │
│   «interface»                                                 │
│   IBoardScorer                                                │
│       ▲                                                       │
│   BoardScorer                      Dictionary                 │
│   (grid scoring math)              (word validation)          │
└───────────────────────────────────────────────────────────────┘
```

---

## ✅ SOLID Principles Applied

### S — Single Responsibility Principle
> *"A class should have only one reason to change."*

Every module in the codebase is scoped to a single, clearly named job. This keeps files small, testable, and independently replaceable.

| Class / File | Sole Responsibility |
|---|---|
| `BoardScorer` | Computes scores purely from grid layout and tile values |
| `Dictionary` | Loads the word list and validates whether a string is a legal word |
| `InputHandler` | Polls SDL events and converts them to game-level input signals |
| `Audio` | Wraps SDL_mixer; plays and stops sounds — nothing else |
| `Renderer` | Wraps SDL2 drawing calls — no game logic |
| `Network` | Wraps SDL_net TCP — no protocol awareness beyond send/receive |
| `Config.hpp` | Holds constants only — no logic |
| `Types.hpp` | Pure data structures — no behaviour |

---

### O — Open/Closed Principle
> *"Open for extension, closed for modification."*

`IGameState` defines a fixed contract (`handleInput / update / render / onEnter / onExit`). New screens — a tutorial, settings screen, or pause menu — are added as new concrete state classes **without touching `GameEngine`** or any existing state.

```
IGameState  ◄──── SplashState
            ◄──── MultiplayerMenuState
            ◄──── IPInputState
            ◄──── NameInputState
            ◄──── PlayState
            ◄──── GameOverState
            ◄──── [AnyNewState]   ← extend here; nothing else changes
```

`GameEngine` never needs to be modified to accommodate new states — it only knows about `IGameState*`.

---

### L — Liskov Substitution Principle
> *"Subtypes must be substitutable for their base types."*

Every concrete class can be used wherever its abstract base is expected, without altering correctness.

| Base Interface | Implementors | Substitution Guarantee |
|---|---|---|
| `IDrawable` | `Label`, `Button` | Both render correctly wherever an `IDrawable*` is expected |
| `IInteractable` | `Button` | Handles clicks correctly wherever an `IInteractable*` is expected |
| `IGameState` | All 6 state classes | Any state can be pushed/popped on the engine's state stack safely |
| `IAudio` | `Audio` | A `NullAudio` mock could substitute with no engine changes |
| `IRenderer` | `Renderer` | A test renderer could substitute transparently |
| `INetwork` | `Network` | A `MockNetwork` could substitute for offline testing |

`Label` deliberately does **not** implement `IInteractable` — it is display-only — correctly applying LSP alongside ISP rather than forcing a stub `handleClick()`.

---

### I — Interface Segregation Principle
> *"Clients should not be forced to depend on interfaces they do not use."*

The UI layer splits drawing and interaction into two narrow, focused interfaces rather than one bloated `IWidget`:

```
IDrawable            IInteractable
  + draw()             + handleClick()

    ▲                       ▲
    │                       │
  Label                  Button
  (draw only)          (draw + click)
```

`Label` is never burdened with implementing `handleClick()` it doesn't need. The System layer applies the same principle:

```
IAudio      → only audio methods
IRenderer   → only drawing methods
INetwork    → only send/receive methods
```

No system forces callers to take unrelated dependencies.

---

### D — Dependency Inversion Principle
> *"Depend on abstractions, not concretions."*

`main.cpp` is the **composition root**: it constructs every concrete SDL system and injects them upward through abstract interfaces. Nothing in `GameEngine` or any state ever `#include`s a concrete SDL wrapper directly.

```
main.cpp  (composition root)
  │
  ├── new Audio()     ──► injected as  IAudio*
  ├── new Renderer()  ──► injected as  IRenderer*
  └── new Network()   ──► injected as  INetwork*
              │
              ▼
          GameEngine
          (depends only on IAudio*, IRenderer*, INetwork*)
              │
              ▼
          States
          (depend only on IBoardScorer*, IRenderer*, IAudio*)
```

Swapping `Audio` for a `NullAudio` (silent test mode) or `Network` for a `MockNetwork` (offline mode) requires **zero changes** to any game logic or state code.

---

## 🔄 State Machine — Game Flow

```
                    ┌──────────────┐
                    │  SplashState │  ← Application start
                    │  (Main Menu) │
                    └──────┬───────┘
                           │ "Multiplayer"
                           ▼
               ┌───────────────────────┐
               │  MultiplayerMenuState │
               │   Host  |  Join       │
               └────┬──────────┬───────┘
                    │ Host     │ Join
                    │          ▼
                    │   ┌─────────────┐
                    │   │ IPInputState│  ← Type host IP
                    │   └──────┬──────┘
                    │          │
                    ▼          ▼
               ┌───────────────────────┐
               │    NameInputState     │  ← Both players name themselves
               └──────────┬────────────┘
                          │
                          ▼
               ┌───────────────────────┐
               │      PlayState        │  ← Core gameplay loop
               │  Place tiles, score,  │
               │  validate words       │
               └──────────┬────────────┘
                          │ Game ends
                          ▼
               ┌───────────────────────┐
               │    GameOverState      │  ← Win / Tie display
               └───────────────────────┘
```

---

## 📦 Dependencies

| Library | Version | Purpose |
|---|---|---|
| SDL2 | ≥ 2.0.18 | Window management, rendering, event loop |
| SDL2_mixer | ≥ 2.0.4 | WAV / MP3 audio playback |
| SDL2_net | ≥ 2.0.1 | Cross-platform TCP networking |
| SDL2_ttf | ≥ 2.0.18 | TrueType font rendering |
| CMake | ≥ 3.16 | Build system |

---

## 🚀 Build Instructions

### Prerequisites

Install SDL2 and its companion libraries:

```bash
# Ubuntu / Debian
sudo apt install libsdl2-dev libsdl2-mixer-dev libsdl2-net-dev libsdl2-ttf-dev

# macOS (Homebrew)
brew install sdl2 sdl2_mixer sdl2_net sdl2_ttf

# Windows
# Download development libraries from https://libsdl.org and configure CMake paths
```

### Compile & Run

```bash
git clone https://github.com/your-username/words-collide.git
cd words-collide

mkdir build && cd build
cmake ..
make -j$(nproc)

./WordsCollide
```

---

## 🎮 Gameplay Guide

| Step | Action |
|---|---|
| 1 | Launch the executable — the **Main Menu (Splash)** appears |
| 2 | Select **Multiplayer** |
| 3 | Choose **Host** (wait for opponent) or **Join** (enter host IP) |
| 4 | Both players enter their **names** on the Name Input screen |
| 5 | The board appears — **take turns placing tiles** to form valid words |
| 6 | Words are validated against `Dictionary.txt` in real time |
| 7 | Score accumulates based on tile values and board positions |
| 8 | When the board is full or no moves remain, the **Game Over** screen declares the winner |

---

## 🎵 Assets

| File | Type | Used In |
|---|---|---|
| `Dictionary.txt` | Text corpus | `Dictionary` — word validation |
| `arial.ttf` | TrueType font | `Renderer` — all UI text |
| `place.wav` | Audio | Tile placed on board |
| `success.wav` | Audio | Valid word accepted |
| `warning.wav` | Audio | Invalid word or illegal move |
| `win.wav` | Audio | Game over — winner declared |
| `welcome.mp3` | Audio | Background music on splash / menu screens |

---

## 📄 License

This project is for educational and portfolio purposes.

---

*Developed by:
Somajit Deb
|| Student ID: 240238
|| Arrono Paul Turjo
|| Student ID: 240206*
