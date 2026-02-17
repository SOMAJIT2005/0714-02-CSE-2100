# WORDS_COLLIDE
"WORDS_COLLIDE" is a Scrabble-inspired, two-player word game created with C and SDL2. It includes a 10-minute timer, dictionary validation, bonus points, sound effects, and a full graphical interface.

# WORDS_COLLIDE 👾📖

A two-player, timed word-building game built with C and the SDL2 library, inspired by classic notebook word games. Challenge a friend to score points by forming words on a grid within a 10-minute limit!

![Gameplay Screenshot](<screenshots/gameplay.png>) ---

## 📝 Description

Remember sketching grids in notebooks and battling friends to build vocabulary? "WORDS_COLLIDE" brings that cherished childhood memory back to life. Developed purely in C using SDL2 for graphics and sound, this project aims to recapture the simple joy of word discovery and friendly competition in a digital format.

---

## ⭐ Key Features

* **Two-Player Gameplay:** Compete head-to-head.
* **Graphical Interface:** Built entirely with SDL2.
* **Timed Matches:** A 10-minute countdown adds excitement, with audio warnings in the final minute.
* **Multiple Screens:** Includes a Splash Screen, Player Name Input, Main Game Board, and Game Over screen.
* **Word Formation:** Place letters to form words horizontally, vertically (top-down), and vertically (bottom-up).
* **Efficient Dictionary Validation:** Uses a pre-sorted dictionary file loaded into memory and searched using `bsearch` for fast validation.
* **Scoring System:** Basic scoring (1 point per letter) with bonus points for predefined special words.
* **Audio Feedback:** Features background music, sound effects for tile placement, valid words, warnings, and game completion.
* **Game Controls:** Includes "Give Up" and "Play Again" options.
* **Modular Code:** Organized into separate `.c` and `.h` files for better maintainability.

---

## 🖼️ Screenshots

| Splash Screen                                         | Name Input                                        | Gameplay                                              | Game Over                                         |
| :----------------------------------------------------: | :------------------------------------------------: | :----------------------------------------------------: | :------------------------------------------------: |
| ![Splash](<screenshots/splash.png>)            | ![Names](<screenshots/names.png>)     | ![Game](<screenshots/gameplay.png>)            | ![Over](<screenshots/gameover.png>)       |
---

## 🛠️ Technology Stack

* **Language:** C
* **Core Library:** SDL2 (Simple DirectMedia Layer)
* **Extensions:**
    * SDL2\_ttf (Font rendering)
    * SDL2\_mixer (Audio playback - WAV for effects, MP3/OGG for music)
* **Compiler:** GCC (Tested with MinGW on Windows)
* **Build System:** Basic `gcc` command (can be integrated with Make or CMake).

---

## 📂 Project Structure

The project is organized into the following main directories:

* `assets/`: Contains external resources like fonts (`.ttf`), sounds (`.wav`, `.mp3`), and the dictionary (`.txt`).
* `include/`: Contains the main header file (`game.h`) defining structures, globals, and function prototypes.
* `src/`: Contains the C source code files (`main.c`, `initialize.c`, `game_logic.c`, `render.c`).

---

## 🚀 Setup & Build Instructions

### Prerequisites

You need `gcc` (or a compatible C compiler) and the **development libraries** for SDL2, SDL2\_ttf, and SDL2\_mixer installed on your system.

* **Windows (using MinGW):** Download the development libraries for MinGW from the SDL websites ([libsdl.org](https://libsdl.org/), [SDL_ttf](https://github.com/libsdl-org/SDL_ttf/releases), [SDL_mixer](https://github.com/libsdl-org/SDL_mixer/releases)). Set up include and lib paths in your compiler environment or directly in the compile command. You will also need the corresponding `.dll` files (`SDL2.dll`, `SDL2_ttf.dll`, `SDL2_mixer.dll`, and their dependencies like `libfreetype-6.dll`, `zlib1.dll`, etc.) placed next to the final executable.
* **Linux (Debian/Ubuntu):**
    ```bash
    sudo apt-get update
    sudo apt-get install build-essential libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev
    ```
* **macOS (using Homebrew):**
    ```bash
    brew install sdl2 sdl2_ttf sdl2_mixer
    ```

### Compilation

Navigate to the project's root directory in your terminal and run the following `gcc` command:

```bash
gcc -g src/main.c src/initialize.c src/game_logic.c src/render.c -o words_collide -I include $(sdl2-config --cflags) $(sdl2-config --libs) -lSDL2_ttf -lSDL2_mixer


