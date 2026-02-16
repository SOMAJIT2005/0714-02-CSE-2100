# WORDS_COLLIDE: Restructuring Report & Game Guide (ver1 → ver2)

**Course:** CSE 2100 Project Show
**Project:** WORDS_COLLIDE Game Engine
**Date:** February 2026
**Version:** 2.0 (Restructuring Complete)

---

## 📊 Executive Summary

The WORDS_COLLIDE game was transformed from a **flat, monolithic C codebase** (ver1: 1 massive source file) into a **modular, layered architecture** (ver2: isolated modules organized into a layered folder hierarchy under `src/` and `include/`). The restructuring applies industry-standard software engineering principles (SOLID, Dependency Injection, Data Encapsulation) to a project, demonstrating professional code organization.

| Aspect | Before (ver1) | After (ver2) | Change |
| :--- | :--- | :--- | :--- |
| **File organization** | 1 monolithic file in root | Modular hierarchy (`src/`, `include/`) | +2 directories, strict separation |
| **Total C files** | 1 (`game.c`) | 5 decomposed modules | +400% modularity |
| **Header files** | 0 | 6 strictly scoped headers | Complete separation of concerns |
| **State Management** | Global Variables (`window`, etc.) | Dependency Injection (`AppContext`) | Thread-safe, no side effects |
| **Data Security** | Dictionary was globally mutable | `static` private variables in `logic.c` | True Data Encapsulation |
| **Naming Convention** | Procedural (`init_game`) | Simulated Namespaces (`Core_InitGame`) | Vastly improved readability |

---

## 🛠️ What Changed: The Restructuring Summary

### 1. File Reorganization (Physical Restructuring)

| ver1 Location | ver2 Location | Reason |
| :--- | :--- | :--- |
| `game.c` (Constants) | `include/config.h` | Extract magic numbers into centralized constants |
| `game.c` (Structs) | `include/types.h` | Centralize shared type definitions (`GameState`, `AppContext`) |
| `game.c` (Drawing) | `src/graphics.c` | UI rendering should be strictly separated from core logic |
| `game.c` (Validation) | `src/logic.c` | Dictionary loading and scoring isolated for data protection |
| `game.c` (Events) | `src/input.c` | Mouse/Keyboard handling separated by game state |
| `game.c` (Sound) | `src/audio.c` | Audio lifecycle and playback management isolated |
| `game.c` (Loop) | `src/main.c` | Entry point strictly manages initialization and the game loop |

### 2. Code Decomposition (Logical Restructuring)

**`game.c` (Monolith) → Decomposed into 5 specialized modules:**

```text
WORDS_COLLIDE Source (src/)
 ├── logic.c
 │    ├── Dictionary binary search algorithm
 │    ├── Bonus word validation
 │    └── Grid scoring mechanics
 │
 ├── graphics.c
 │    ├── Text & font rendering
 │    ├── UI Button drawing
 │    └── Dynamic visual timers
 │
 ├── input.c
 │    ├── Mouse collision detection
 │    ├── Keyboard text input processing
 │    └── State machine transitions
 │
 ├── audio.c
 │    └── Mix_Chunk & Mix_Music lifecycle management
 │
 └── main.c
      └── Game loop, frame timing, and module delegation
```

---

## 🏛️ Architectural Improvements

### Layered Architecture Model

The codebase now strictly adheres to a layered design. Higher layers (UI/IO) depend on the Engine Layer, and all layers depend on the Foundation Layer. Data flows downwards, preventing circular dependencies.

```text
+---------------------------------------------------+
|                 UI & I/O Layer                    |
|                                                   |
|  +---------------+ +------------+ +------------+  |
|  |  Graphics     | |   Audio    | |   Input    |  |
|  | (graphics.c)  | | (audio.c)  | | (input.c)  |  |
|  +-------+-------+ +-----+------+ +-----+------+  |
+----------|---------------|--------------|---------+
           |               |              |
           v               v              v
+---------------------------------------------------+
|                 Engine Layer                      |
|                                                   |
|          +-----------------------------+          |
|          | Game Logic & Dictionary     |          |
|          | (logic.c)                   |          |
|          +-------------+---------------+          |
+------------------------|--------------------------+
                         |
                         v
+---------------------------------------------------+
|               Foundation Layer                    |
|                                                   |
|  +---------------------------------------------+  |
|  | Types & Constants                           |  |
|  | (types.h, config.h)                         |  |
|  +---------------------------------------------+  |
+---------------------------------------------------+
```

---

## 🚀 Developer & User Guide

### 📂 Folder Structure

```text
WORDS_COLLIDE/
├── assets/                 # Fonts, Dictionary.txt, .wav, and .mp3 files
├── include/                # Header files (.h)
│   ├── audio.h             
│   ├── config.h            
│   ├── graphics.h          
│   ├── input.h             
│   ├── logic.h             
│   └── types.h             
├── src/                    # Source files (.c)
│   ├── audio.c             
│   ├── graphics.c          
│   ├── input.c             
│   ├── logic.c             
│   └── main.c              
└── README.md               # This documentation
```

### ⚙️ Prerequisites
* **C Compiler:** MinGW-w64 (32-bit `i686` architecture recommended for optimal SDL2 compatibility).
* **Libraries:** SDL2, SDL2_ttf, SDL2_mixer.
* **DLLs:** Ensure `SDL2.dll`, `SDL2_ttf.dll`, and `SDL2_mixer.dll` are in the root folder alongside the compiled executable.

### 🔨 How to Build

**Using VS Code (`tasks.json`)**
Press `Ctrl + Shift + B`. *(Ensure your tasks.json is configured to compile `${workspaceFolder}\\src\\*.c`)*

**Using Command Line (Windows PowerShell)**
Navigate to the root `WORDS_COLLIDE` directory and run:

```powershell
gcc src/*.c -o main.exe -I"E:/SDL2-2.32.0/i686-w64-mingw32/include" -L"E:/SDL2-2.32.0/i686-w64-mingw32/lib" -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer
```

### 🎮 How to Play

1. **Launch:** Run `./main.exe`.
2. **Setup:** Click "Start Game", enter Player 1 and Player 2 names, and press `ENTER`.
3. **Gameplay:** * Click an empty tile to select it.
   * Type a single letter on your keyboard.
   * Press `ENTER` to lock the letter in. 
4. **Scoring:** Form valid words horizontally or vertically to score points based on length. Discovering predefined "Bonus Words" grants +15 points!
5. **Turn Timer:** You have exactly 20 seconds to make a move (tracked by the green bar). If time runs out, your turn is skipped.
6. **Winning:** The game lasts for exactly 10 minutes. The highest score wins!