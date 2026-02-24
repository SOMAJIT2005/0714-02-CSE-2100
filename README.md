# 🔤 WORDS COLLIDE

> **A 2-Player Real-Time Word Formation Board Game — Built in C with SDL2**
>
> **Course:** CSE 2100 Project Show &nbsp;|&nbsp; **Student:** 240238 &nbsp;|&nbsp; **Date:** February 2026

---

## 📑 Table of Contents

1. [Project Overview](#-project-overview)
2. [Version Comparison at a Glance](#-version-comparison-at-a-glance)
3. [What Changed: ver1 → ver2 (Full Code Analysis)](#-what-changed-ver1--ver2-full-code-analysis)
   - [File Structure](#1-file-structure)
   - [Global Variables Eliminated](#2-global-variables-eliminated--appcontext-introduced)
   - [Enum Naming](#3-enum-naming)
   - [Struct Field Naming](#4-struct-field-naming-snake_case--camelcase)
   - [Function Naming & Namespacing](#5-function-naming--namespacing)
   - [Logic Algorithm Improvement](#6-logic-algorithm-improvement)
   - [Audio: Direct Calls → Wrapper API](#7-audio-direct-sdl2-calls--wrapper-api)
   - [Cleanup Safety](#8-cleanup-null-safety)
   - [Render Function Signatures](#9-render-function-signatures)
   - [Static Scope (Information Hiding)](#10-static-scope-information-hiding)
4. [Architecture](#-architecture)
5. [Module Breakdown](#-module-breakdown)
6. [File Structure](#-file-structure)
7. [How to Build](#️-how-to-build)
8. [How to Play](#-how-to-play)
9. [Scoring System](#-scoring-system)
10. [AI Refactoring Prompts: ver1 → ver2](#-ai-refactoring-prompts-ver1--ver2)
11. [Design Principles Applied](#-design-principles-applied)

---

## 🎯 Project Overview

**WORDS COLLIDE** is a 2-player competitive word game rendered in real-time using SDL2. Players take turns placing letters on a 20×20 shared grid, forming valid English words horizontally or vertically. Special *bonus words* grant extra points, and every move is bounded by a 20-second turn timer. The game runs for exactly 10 minutes.

The project was developed in two distinct phases:

| Version | Folder | Description |
|---|---|---|
| **ver1** | `WORDS_COLLIDE_OLD/` | Single monolithic `game.c` — 632 lines, 11 global variables |
| **ver2** | `WORDS_COLLIDE/` | Modular architecture — 5 source files, 6 headers, 0 global variables |

---

## 📊 Version Comparison at a Glance

| Metric | ver1 (`game.c`) | ver2 (Modular) | Change |
|---|---|---|---|
| **Source files (.c)** | 1 | 5 | +400% modularity |
| **Header files (.h)** | 0 | 6 | Full separation of concerns |
| **Total lines of code** | 632 | 721 across all modules | Distributed & maintainable |
| **Global variables** | **11** (`window`, `renderer`, `font`, `font_large`, 4× sounds, 1× music, `giant_dictionary`, `actual_word_count`) | **0** | Complete elimination |
| **Naming convention** | `snake_case` flat | `PascalCase` namespaced | Industry standard |
| **Struct fields** | `snake_case` | `camelCase` with `is*` boolean prefix | Consistent style |
| **Enum prefix** | `GAME_STATE_*` | `STATE_*` | Shorter, cleaner |
| **Audio calls** | 8× raw `Mix_*` scattered across 3 functions | 3 clean wrappers (`Audio_PlaySound`, `Audio_PlayMusic`, `Audio_StopMusic`) | Encapsulated |
| **Dictionary storage** | `giant_dictionary[][]` — globally mutable | `loadedDictionary[][]` — `static` in `logic.c` | Data encapsulation |
| **Cleanup safety** | No null checks — unsafe free | All resources null-checked before free | Crash-safe |
| **Context passing** | Functions read globals directly | `AppContext*` passed explicitly | Dependency Injection |

---

## 🔄 What Changed: ver1 → ver2 (Full Code Analysis)

### 1. File Structure

**ver1 — Everything in one flat file:**
```
WORDS_COLLIDE_OLD/
├── arial.ttf
├── Dictionary.txt
├── game.c          ← 632 lines: constants, structs, globals,
│                     logic, rendering, input, audio, main loop
├── place.wav
├── success.wav
├── warning.wav
├── welcome.mp3
└── win.wav
```

**ver2 — Organized into layers:**
```
WORDS_COLLIDE/
├── assets/             ← arial.ttf, Dictionary.txt, *.wav, *.mp3
├── include/            ← 6 header files
│   ├── config.h        ← All #define constants (GRID_SIZE, timers, etc.)
│   ├── types.h         ← All structs & enums (AppContext, GameState, Button)
│   ├── audio.h
│   ├── graphics.h
│   ├── input.h
│   └── logic.h
└── src/                ← 5 source files (721 lines total)
    ├── main.c          ← 151 lines — entry point, game loop, SDL init
    ├── logic.c         ← 168 lines — dictionary engine & scoring
    ├── graphics.c      ← 215 lines — all SDL2 rendering
    ├── input.c         ← 146 lines — event handling & state machine
    └── audio.c         ←  41 lines — SDL2_mixer management
```

---

### 2. Global Variables Eliminated → `AppContext` Introduced

**ver1 — 11 globals polluting the entire program (`game.c` lines 65–80):**
```c
// Accessible and modifiable by any function — no encapsulation
SDL_Window   *window           = NULL;
SDL_Renderer *renderer         = NULL;
TTF_Font     *font             = NULL;
TTF_Font     *font_large       = NULL;
Mix_Chunk    *sound_place_tile = NULL;
Mix_Chunk    *sound_valid_word = NULL;
Mix_Chunk    *sound_warning    = NULL;
Mix_Chunk    *sound_win        = NULL;
Mix_Music    *music_welcome    = NULL;
char          giant_dictionary[MAX_WORDS_IN_DICT][MAX_WORD_LENGTH];
int           actual_word_count = 0;
```

**ver2 — Zero globals. All SDL handles bundled into `AppContext` in `types.h`:**
```c
// types.h — inject exactly what each function needs, nothing more
typedef struct {
    SDL_Window   *window;
    SDL_Renderer *renderer;
    TTF_Font     *fontRegular;
    TTF_Font     *fontLarge;
    Mix_Chunk    *sfxPlaceTile;
    Mix_Chunk    *sfxValidWord;
    Mix_Chunk    *sfxWarning;
    Mix_Chunk    *sfxWin;
    Mix_Music    *bgmWelcome;
} AppContext;
```
Functions now receive exactly what they need via `AppContext*` — no hidden dependencies.

---

### 3. Enum Naming

The game state enum was simplified by removing the redundant `GAME_` prefix — the type name `GameState` already provides full context:

| ver1 (`game.c`) | ver2 (`types.h`) |
|---|---|
| `GAME_STATE_SPLASH` | `STATE_SPLASH` |
| `GAME_STATE_GET_NAMES` | `STATE_GET_NAMES` |
| `GAME_STATE_PLAYING` | `STATE_PLAYING` |
| `GAME_STATE_GAME_OVER` | `STATE_GAME_OVER` |
| `GAME_STATE_RESTART` | `STATE_RESTART` |
| `GAME_STATE_QUIT` | `STATE_QUIT` |

---

### 4. Struct Field Naming: `snake_case` → `camelCase`

All `GameState` and `Button` struct fields were renamed from C-style `snake_case` to consistent `camelCase`. Booleans received an `is` prefix for clarity:

| ver1 Field | ver2 Field | Note |
|---|---|---|
| `game_state` | `currentState` | More descriptive |
| `player_names[2]` | `playerNames[2]` | camelCase |
| `current_name_input` | `currentNameInput` | camelCase |
| `game_start_time` | `gameStartTime` | camelCase |
| `last_warning_time` | `lastWarningTime` | camelCase |
| `turn_start_time` | `turnStartTime` | camelCase |
| `current_player` | `currentPlayer` | camelCase |
| `current_letter` | `currentLetter` | camelCase |
| `selected_x` | `selectedX` | camelCase |
| `selected_y` | `selectedY` | camelCase |
| `tile_selected` | **`isTileSelected`** | Boolean `is` prefix added |
| `scored_word_count` | `scoredWordCount` | camelCase |
| `scored_words` | `scoredWords` | camelCase |
| `give_up_button` | `giveUpButton` | camelCase |
| `play_again_button` | `playAgainButton` | camelCase |
| `start_game_button` | `startGameButton` | camelCase |
| `hovered` (Button field) | **`isHovered`** (Button field) | Boolean `is` prefix added |

---

### 5. Function Naming & Namespacing

Every function was renamed from flat `snake_case` to **simulated namespace** format: `Module_ActionDescription()`. Internal helpers were also marked `static`.

| ver1 (flat) | ver2 (namespaced) | Module | Visibility |
|---|---|---|---|
| `init_sdl()` | `Core_InitSDL()` | `main.c` | `static` |
| `init_game()` | `Core_InitGame()` | `main.c` | `static` |
| `cleanup()` | `Core_Cleanup()` | `main.c` | `static` |
| `load_dictionary()` | `Logic_LoadDictionary()` | `logic.c` | public |
| `is_valid_word()` | `Logic_IsValidWord()` | `logic.c` | public |
| `check_words_and_score()` | `Logic_CheckAndScore()` | `logic.c` | public |
| `is_word_already_scored()` | `Logic_IsWordAlreadyScored()` | `logic.c` | `static` |
| `calculate_word_score()` | `Logic_CalculateWordScore()` | `logic.c` | `static` |
| `is_bonus_word()` | `Logic_IsBonusWord()` | `logic.c` | `static` |
| `compare_strings()` | `Logic_CompareStrings()` | `logic.c` | `static` |
| `is_mouse_over_button()` | `Input_IsMouseOverButton()` | `input.c` | `static` |
| `handle_splash_input()` | `Input_HandleSplash()` | `input.c` | public |
| `handle_name_input()` | `Input_HandleNames()` | `input.c` | public |
| `handle_game_input()` | `Input_HandleGame()` | `input.c` | public |
| `handle_game_over_input()` | `Input_HandleGameOver()` | `input.c` | public |
| `render_text()` | `Graphics_DrawText()` | `graphics.c` | `static` |
| `render_text_centered()` | `Graphics_DrawTextCentered()` | `graphics.c` | `static` |
| `render_button()` | `Graphics_DrawButton()` | `graphics.c` | `static` |
| `render_splash_screen()` | `Graphics_RenderSplashScreen()` | `graphics.c` | public |
| `render_name_input()` | `Graphics_RenderNameInput()` | `graphics.c` | public |
| `render_game()` | `Graphics_RenderGame()` | `graphics.c` | public |
| `render_game_over()` | `Graphics_RenderGameOver()` | `graphics.c` | public |

Internal dictionary variables were also renamed for clarity and moved to `static` scope:

| ver1 (global) | ver2 (static in `logic.c`) |
|---|---|
| `giant_dictionary[][]` | `loadedDictionary[][]` |
| `actual_word_count` | `totalLoadedWords` |
| `bonus_words[]` | `specialBonusWords[]` |
| `bonus_word_count` | `totalBonusWords` |

---

### 6. Logic Algorithm Improvement

The horizontal/vertical word-scanning loop in `Logic_CheckAndScore()` was rewritten for readability. The ver1 loop used a compact but cryptic combined loop condition; ver2 resolves the tile first, then uses a clean early `break`:

**ver1 — Compact but difficult to reason about:**
```c
// Condition mixes "grid cell not empty OR this is the placed tile" — confusing
for (int i = h_start; i < GRID_SIZE && (game->grid[i][y] != '\0' || i == x); i++) {
    if (h_len >= MAX_WORD_LENGTH - 1) break;
    h_word[h_len++] = (i == x) ? letter : game->grid[i][y];
}
```

**ver2 — Readable with named variables and an explicit `break` on blank:**
```c
for (int col = startColumn; col < GRID_SIZE; col++) {
    char currentTile = (col == targetCol) ? placedLetter : game->grid[col][targetRow];

    if (currentTile == '\0') break;  // Blank space ends the word

    if (currentLength < MAX_WORD_LENGTH - 1) {
        horizontalWord[currentLength++] = currentTile;
    }
}
```

Local variable names also improved throughout:

| ver1 local | ver2 local |
|---|---|
| `h_word`, `v_word` | `horizontalWord`, `verticalWord` |
| `h_start`, `v_start` | `startColumn`, `startRow` |
| `h_len`, `v_len` | `currentLength` (reused clearly) |
| `x`, `y` parameters | `targetCol`, `targetRow` parameters |
| `letter` parameter | `placedLetter` parameter |

---

### 7. Audio: Direct SDL2 Calls → Wrapper API

**ver1 — 8 raw `Mix_*` calls scattered across 3 different functions:**
```c
// In handle_game_input():
Mix_HaltMusic();
Mix_PlayChannel(-1, sound_win, 0);
Mix_PlayChannel(-1, sound_place_tile, 0);
Mix_PlayChannel(-1, sound_valid_word, 0);

// In main game loop:
Mix_HaltMusic();
Mix_PlayChannel(-1, sound_win, 0);
Mix_PlayChannel(-1, sound_warning, 0);
```

**ver2 — 3 clean wrappers in `audio.c`, called consistently everywhere:**
```c
// audio.c — the entire public audio API
void Audio_PlaySound(Mix_Chunk *sound) { if (sound) Mix_PlayChannel(-1, sound, 0); }
void Audio_PlayMusic(Mix_Music *music) { if (music) Mix_PlayMusic(music, -1);      }
void Audio_StopMusic(void)             { Mix_HaltMusic();                           }

// Usage in input.c and main.c — intention-revealing and consistent
Audio_StopMusic();
Audio_PlaySound(app->sfxWin);
Audio_PlaySound(app->sfxPlaceTile);
Audio_PlaySound(app->sfxValidWord);
```

The wrappers also add a null guard (`if (sound)`) that ver1 lacked, preventing crashes if an asset fails to load.

---

### 8. Cleanup Null Safety

**ver1 — Frees all resources unconditionally (crash risk if init partially failed):**
```c
void cleanup() {
    Mix_FreeChunk(sound_place_tile);   // ← crash if WAV failed to load
    Mix_FreeChunk(sound_valid_word);   // ← crash if WAV failed to load
    TTF_CloseFont(font);               // ← crash if font not loaded
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
```

**ver2 — Every resource null-checked before freeing:**
```c
void Audio_Cleanup(AppContext *app) {
    if (app->sfxPlaceTile) Mix_FreeChunk(app->sfxPlaceTile);  // ✓ safe
    if (app->sfxValidWord) Mix_FreeChunk(app->sfxValidWord);  // ✓ safe
    if (app->sfxWarning)   Mix_FreeChunk(app->sfxWarning);    // ✓ safe
    if (app->sfxWin)       Mix_FreeChunk(app->sfxWin);        // ✓ safe
    if (app->bgmWelcome)   Mix_FreeMusic(app->bgmWelcome);    // ✓ safe
    Mix_Quit();
}

static void Core_Cleanup(AppContext *app) {
    SDL_StopTextInput();
    Audio_Cleanup(app);                                         // delegated
    if (app->fontRegular) TTF_CloseFont(app->fontRegular);    // ✓ safe
    if (app->fontLarge)   TTF_CloseFont(app->fontLarge);      // ✓ safe
    if (app->renderer)    SDL_DestroyRenderer(app->renderer); // ✓ safe
    if (app->window)      SDL_DestroyWindow(app->window);     // ✓ safe
    TTF_Quit();
    SDL_Quit();
}
```

---

### 9. Render Function Signatures

In ver1, render functions silently read `renderer`, `font`, and `font_large` from globals. In ver2, they explicitly receive `AppContext*` — making all dependencies visible in the signature:

| ver1 signature | ver2 signature |
|---|---|
| `render_splash_screen(GameState *game)` | `Graphics_RenderSplashScreen(AppContext *app, GameState *game)` |
| `render_name_input(GameState *game)` | `Graphics_RenderNameInput(AppContext *app, GameState *game)` |
| `render_game(GameState *game)` | `Graphics_RenderGame(AppContext *app, GameState *game)` |
| `render_game_over(GameState *game)` | `Graphics_RenderGameOver(AppContext *app, GameState *game)` |
| `render_button(Button *button)` | `Graphics_DrawButton(AppContext *app, Button *button)` |
| `render_text(TTF_Font*, const char*, int, int, SDL_Color)` | `Graphics_DrawText(AppContext *app, TTF_Font*, const char*, int, int, SDL_Color)` |

---

### 10. Static Scope (Information Hiding)

ver2 explicitly marks internal functions and data `static`, preventing accidental linkage from other translation units:

| Symbol | ver1 scope | ver2 scope |
|---|---|---|
| `loadedDictionary[][]` | global | `static` in `logic.c` |
| `totalLoadedWords` | global | `static` in `logic.c` |
| `specialBonusWords[]` | global | `static const` in `logic.c` |
| `totalBonusWords` | global | `static const int` in `logic.c` |
| `Logic_CompareStrings()` | global linkage | `static` in `logic.c` |
| `Logic_IsWordAlreadyScored()` | global linkage | `static` in `logic.c` |
| `Logic_CalculateWordScore()` | global linkage | `static` in `logic.c` |
| `Logic_IsBonusWord()` | global linkage | `static` in `logic.c` |
| `Input_IsMouseOverButton()` | global linkage | `static` in `input.c` |
| `Graphics_DrawText()` | global linkage | `static` in `graphics.c` |
| `Graphics_DrawTextCentered()` | global linkage | `static` in `graphics.c` |
| `Graphics_DrawButton()` | global linkage | `static` in `graphics.c` |
| `Core_InitSDL()` | global `init_sdl()` | `static` in `main.c` |
| `Core_InitGame()` | global `init_game()` | `static` in `main.c` |
| `Core_Cleanup()` | global `cleanup()` | `static` in `main.c` |

---

## 🏛️ Architecture

```
╔══════════════════════════════════════════════════════════════╗
║                      UI & I/O Layer                          ║
║                                                              ║
║  ┌──────────────┐    ┌──────────────┐    ┌───────────────┐  ║
║  │  graphics.c  │    │   audio.c    │    │   input.c     │  ║
║  │              │    │              │    │               │  ║
║  │ DrawText()   │    │ PlaySound()  │    │ HandleSplash()│  ║
║  │ DrawButton() │    │ PlayMusic()  │    │ HandleNames() │  ║
║  │ RenderGame() │    │ StopMusic()  │    │ HandleGame()  │  ║
║  │ RenderOver() │    │ Cleanup()    │    │ HandleOver()  │  ║
║  └──────┬───────┘    └──────┬───────┘    └──────┬────────┘  ║
╚═════════╪═══════════════════╪═══════════════════╪═══════════╝
          │                   │                   │
          ▼                   ▼                   ▼
╔══════════════════════════════════════════════════════════════╗
║                      Engine Layer                            ║
║                                                              ║
║   ┌────────────────────────────────────────────────────┐    ║
║   │                    logic.c                          │    ║
║   │                                                      │    ║
║   │  Logic_LoadDictionary()  — load & sort words         │    ║
║   │  Logic_IsValidWord()     — binary search O(log n)    │    ║
║   │  Logic_CheckAndScore()   — scan grid, award points   │    ║
║   │  [static] IsBonusWord(), CalculateWordScore()        │    ║
║   │  [static] loadedDictionary[][] — fully encapsulated  │    ║
║   └──────────────────────────┬─────────────────────────┘    ║
╚══════════════════════════════╪═══════════════════════════════╝
                               │
                               ▼
╔══════════════════════════════════════════════════════════════╗
║                    Foundation Layer                          ║
║                                                              ║
║  ┌────────────────────────┐   ┌──────────────────────────┐  ║
║  │       types.h          │   │        config.h           │  ║
║  │                        │   │                           │  ║
║  │  typedef AppContext    │   │  WINDOW_WIDTH    800      │  ║
║  │  typedef GameState     │   │  GRID_SIZE        20      │  ║
║  │  typedef Button        │   │  TILE_SIZE        40      │  ║
║  │  enum   GameStateEnum  │   │  TURN_DURATION_MS 20000   │  ║
║  │                        │   │  GAME_DURATION_MS 600000  │  ║
║  │                        │   │  WARNING_TIME_MS  50000   │  ║
║  │                        │   │  BONUS_WORD_POINTS   15   │  ║
║  └────────────────────────┘   └──────────────────────────┘  ║
╚══════════════════════════════════════════════════════════════╝
```

**Dependency rule:** UI/IO → Engine → Foundation. No upward dependencies. No circular includes.

---

## 🧩 Module Breakdown

### `src/main.c` — 151 lines
Entry point. Owns `Core_InitSDL()`, `Core_InitGame()`, `Core_Cleanup()`. Runs the game loop, handles both the 10-minute match timer and the 20-second per-turn timer. Dispatches events and rendering through `switch(game.currentState)`.

### `src/logic.c` — 168 lines
The game brain. Loads `Dictionary.txt` into a `static` private array and normalizes all words to uppercase. `Logic_IsValidWord()` uses `bsearch()` for **O(log n)** lookup. `Logic_CheckAndScore()` scans both horizontal and vertical axes from the placed tile, validates formed words, checks the secret bonus word list, accumulates score, and tracks already-scored words to prevent double-counting.

### `src/graphics.c` — 215 lines
Pure rendering — no game logic. Three `static` helpers (`Graphics_DrawText`, `Graphics_DrawTextCentered`, `Graphics_DrawButton`) are used by four public render functions, one per game state. Renders the turn timer as a shrinking green bar over a red background, and flashes the game clock red when under 50 seconds remaining.

### `src/input.c` — 146 lines
Event handling and game state transitions. `Input_HandleGame()` commits a letter to the grid on `ENTER`, calls `Logic_CheckAndScore()`, triggers audio feedback via wrappers, then switches `currentPlayer`. Mouse collision detection is handled by the `static` helper `Input_IsMouseOverButton()`.

### `src/audio.c` — 41 lines
Minimal and focused. `Audio_Init()` opens the SDL2 mixer and loads all 4 sound effects + 1 music track into `AppContext`. Three wrapper functions expose a clean, null-safe public API. `Audio_Cleanup()` null-checks every handle before freeing.

---

## 📂 File Structure

```
0714-02-CSE-2100/
│
├── WORDS_COLLIDE/               ← ver2 (Current, Modular)
│   ├── assets/
│   │   ├── Dictionary.txt       ← Word list (binary-searched at runtime)
│   │   ├── arial.ttf            ← Font (24pt regular, 48pt large)
│   │   ├── place.wav            ← Sound: tile placed
│   │   ├── success.wav          ← Sound: valid word formed
│   │   ├── warning.wav          ← Sound: time running out
│   │   ├── win.wav              ← Sound: game over
│   │   └── welcome.mp3          ← BGM: splash & name entry
│   ├── include/
│   │   ├── config.h             ← All constants (#define)
│   │   ├── types.h              ← AppContext, GameState, Button
│   │   ├── audio.h
│   │   ├── graphics.h
│   │   ├── input.h
│   │   └── logic.h
│   └── src/
│       ├── main.c
│       ├── logic.c
│       ├── graphics.c
│       ├── input.c
│       └── audio.c
│
├── WORDS_COLLIDE_OLD/           ← ver1 (Legacy, Monolithic)
│   ├── game.c                   ← Single 632-line monolith
│   ├── arial.ttf
│   ├── Dictionary.txt
│   ├── place.wav / success.wav / warning.wav / win.wav / welcome.mp3
│
└── README.md
```

---

## ⚙️ How to Build

### Prerequisites

| Requirement | Detail |
|---|---|
| **Compiler** | MinGW-w64 32-bit (`i686-w64-mingw32`) |
| **SDL2** | Core library |
| **SDL2_ttf** | Font rendering |
| **SDL2_mixer** | Audio playback |
| **DLLs** | `SDL2.dll`, `SDL2_ttf.dll`, `SDL2_mixer.dll` alongside the `.exe` |

### Via VS Code
Press **`Ctrl + Shift + B`** — requires `tasks.json` configured to compile `${workspaceFolder}\src\*.c`.

### Via PowerShell (Windows)
```powershell
cd WORDS_COLLIDE

gcc src/*.c -o main.exe `
    -Iinclude `
    -I"E:/SDL2-2.32.0/i686-w64-mingw32/include" `
    -L"E:/SDL2-2.32.0/i686-w64-mingw32/lib" `
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer
```

> 💡 Adjust the SDL2 `-I` and `-L` paths to match your local installation directory.

---

## 🎮 How to Play

| Step | Action |
|---|---|
| **1. Launch** | Run `./main.exe` |
| **2. Start** | Click the **Start Game** button on the splash screen |
| **3. Names** | Type Player 1 name → `ENTER` → Type Player 2 name → `ENTER` |
| **4. Select Tile** | Click any empty square on the 20×20 grid |
| **5. Type Letter** | Press a letter key (automatically uppercased) |
| **6. Confirm** | Press `ENTER` to lock the letter into the grid |
| **7. Score** | Valid words formed horizontally or vertically earn points |
| **8. Turn Timer** | You have **20 seconds** — the green bar depletes in real time |
| **9. Win** | Game ends after **10 minutes** — highest score wins! |

- If your turn timer expires, your turn is **skipped automatically** and switched to the other player
- Click **Give Up** at any time to end the game early
- On the Game Over screen, click **Play Again** to restart, or press any key to quit

---

## 📈 Scoring System

| Condition | Points |
|---|---|
| Valid word of N letters | **+N** |
| 3-letter valid word | **+3** |
| 5-letter valid word | **+5** |
| **Bonus word** (N letters) | **+N + 15** |
| Turn expired (no move made) | **±0** |
| Same word matched twice | **0** — duplicate prevention active |

**Bonus Words** (hardcoded as `static const` in `logic.c`):

```
BONUS  |  EXTRA  |  SPECIAL  |  MAGIC  |  EYE  |  GREEN
```

Words are validated against `Dictionary.txt` using **binary search** (`bsearch()`) — O(log n). All dictionary entries are uppercased on load. Already-scored words are tracked in `game->scoredWords[]` and automatically skipped on re-detection.

---

## 🤖 AI Refactoring Prompts: ver1 → ver2

These are precise prompts that I have used to make the vertion 2.

---

### Prompt 1 — Split monolith into modules

```
I have a single monolithic C file called game.c that contains constants, structs,
global variables, game logic, rendering, input handling, audio management, and the
main loop all in one place.

Refactor it into the following modular file structure without changing any logic:

  include/config.h    — all #define constants only
  include/types.h     — all typedef structs and enums only
  include/logic.h     — declarations for logic functions
  include/graphics.h  — declarations for graphics functions
  include/input.h     — declarations for input functions
  include/audio.h     — declarations for audio functions
  src/logic.c         — dictionary loading, word validation, scoring
  src/graphics.c      — all SDL2 rendering functions
  src/input.c         — all SDL event handling functions
  src/audio.c         — all SDL2_mixer functions
  src/main.c          — main(), SDL init, game loop only

Rules:
- Do not change any logic, algorithms, or values
- Each .c file must #include only the headers it actually needs
- No global variables — carry shared SDL state in a struct called AppContext defined in types.h
- Header files must use include guards (#ifndef / #define / #endif)

Here is the full game.c:
[paste game.c here]
```

---

### Prompt 2 — Eliminate global variables with `AppContext`

```
The following C file has 11 global variables:
  SDL_Window*, SDL_Renderer*, two TTF_Font*, four Mix_Chunk*, one Mix_Music*,
  the dictionary array, and the word count integer.

Refactor by:
1. Creating a typedef struct called AppContext in a header file types.h that holds
   all SDL/audio/font handles as named fields:
     window, renderer, fontRegular, fontLarge,
     sfxPlaceTile, sfxValidWord, sfxWarning, sfxWin, bgmWelcome

2. Removing all 11 global variable declarations

3. Declaring AppContext app in main() with zero-initialization: AppContext app = {0};

4. Updating every function that previously read from globals to instead accept
   AppContext* as its first parameter and access state through that pointer

5. The dictionary array and word count must NOT go into AppContext — they belong
   as static variables inside logic.c (encapsulated, not injectable)

Do not change any logic. Show the updated function signatures and the AppContext struct.

Here is the code:
[paste game.c here]
```

---

### Prompt 3 — Rename enums from `GAME_STATE_*` to `STATE_*`

```
In the following C code, rename all game state enum values by removing the
redundant GAME_ prefix. Apply the rename consistently everywhere the values appear
— in the enum definition, switch statements, if-conditions, and assignments.

Rename map:
  GAME_STATE_SPLASH    → STATE_SPLASH
  GAME_STATE_GET_NAMES → STATE_GET_NAMES
  GAME_STATE_PLAYING   → STATE_PLAYING
  GAME_STATE_GAME_OVER → STATE_GAME_OVER
  GAME_STATE_RESTART   → STATE_RESTART
  GAME_STATE_QUIT      → STATE_QUIT

Also rename the field inside the GameState struct:
  game_state → currentState

Update every usage of game->game_state and game.game_state to match.
Do not change anything else.

Here is the code:
[paste game.c here]
```

---

### Prompt 4 — Rename all struct fields to `camelCase` with `is` boolean prefix

```
Rename every field in the GameState and Button structs from snake_case to camelCase.
Boolean fields must also receive an "is" prefix. Apply every rename consistently
across all usages in the entire file.

GameState field renames:
  player_names        → playerNames
  current_name_input  → currentNameInput
  game_start_time     → gameStartTime
  last_warning_time   → lastWarningTime
  turn_start_time     → turnStartTime
  current_player      → currentPlayer
  current_letter      → currentLetter
  selected_x          → selectedX
  selected_y          → selectedY
  tile_selected       → isTileSelected      ← boolean: add "is" prefix
  scored_word_count   → scoredWordCount
  scored_words        → scoredWords
  give_up_button      → giveUpButton
  play_again_button   → playAgainButton
  start_game_button   → startGameButton

Button field renames:
  hovered             → isHovered           ← boolean: add "is" prefix

Do not change any logic, values, or function names. Only rename struct fields.

Here is the code:
[paste game.c here]
```

---

### Prompt 5 — Rename all functions to `Module_ActionDescription` namespaced style

```
Rename every function in this C program from flat snake_case to a namespaced style
using the format Module_ActionName (PascalCase after the underscore).
Apply every rename consistently at both the definition site and all call sites.

Rename map:
  init_sdl()                → Core_InitSDL()
  init_game()               → Core_InitGame()
  cleanup()                 → Core_Cleanup()
  load_dictionary()         → Logic_LoadDictionary()
  is_valid_word()           → Logic_IsValidWord()
  is_word_already_scored()  → Logic_IsWordAlreadyScored()
  calculate_word_score()    → Logic_CalculateWordScore()
  check_words_and_score()   → Logic_CheckAndScore()
  is_bonus_word()           → Logic_IsBonusWord()
  compare_strings()         → Logic_CompareStrings()
  is_mouse_over_button()    → Input_IsMouseOverButton()
  handle_splash_input()     → Input_HandleSplash()
  handle_name_input()       → Input_HandleNames()
  handle_game_input()       → Input_HandleGame()
  handle_game_over_input()  → Input_HandleGameOver()
  render_text()             → Graphics_DrawText()
  render_text_centered()    → Graphics_DrawTextCentered()
  render_button()           → Graphics_DrawButton()
  render_splash_screen()    → Graphics_RenderSplashScreen()
  render_name_input()       → Graphics_RenderNameInput()
  render_game()             → Graphics_RenderGame()
  render_game_over()        → Graphics_RenderGameOver()

Also rename these internal data variables:
  giant_dictionary[][]  → loadedDictionary[][]
  actual_word_count     → totalLoadedWords
  bonus_words[]         → specialBonusWords[]
  bonus_word_count      → totalBonusWords

Do not change any logic or struct fields.

Here is the code:
[paste game.c here]
```

---

### Prompt 6 — Improve the word-scanning loop in the scoring function

```
In the function check_words_and_score() (or Logic_CheckAndScore() if already renamed),
the horizontal and vertical word-scanning for loops use a complex combined condition
that is hard to read:

  // Current (hard to read):
  for (int i = h_start; i < GRID_SIZE && (game->grid[i][y] != '\0' || i == x); i++) {
      if (h_len >= MAX_WORD_LENGTH - 1) break;
      h_word[h_len++] = (i == x) ? letter : game->grid[i][y];
  }

Rewrite both the horizontal and vertical loops using this cleaner pattern:
1. Resolve the current tile into a named local variable (currentTile) at the top of the loop body
2. Use an explicit early break when currentTile == '\0' instead of encoding it in the loop condition
3. Rename all short/abbreviated local variables to descriptive full names:
     h_word  → horizontalWord    v_word  → verticalWord
     h_start → startColumn       v_start → startRow
     h_len   → currentLength     v_len   → currentLength  (reuse clearly)
     x, y params → targetCol, targetRow
     letter param → placedLetter

The logic outcome (which words are found and scored) must be identical.
Do not change anything outside these two loops.

Here is the function:
[paste check_words_and_score() here]
```

---

### Prompt 7 — Replace raw `Mix_*` audio calls with a wrapper API

```
Currently, SDL2_mixer functions (Mix_PlayChannel, Mix_HaltMusic, Mix_PlayMusic) are
called directly in 3 different functions across the file — 8 call sites total.

Create a small audio wrapper module with these 3 functions in audio.c:

  void Audio_PlaySound(Mix_Chunk *sound)  — null-checks sound, then calls Mix_PlayChannel(-1, sound, 0)
  void Audio_PlayMusic(Mix_Music *music)  — null-checks music, then calls Mix_PlayMusic(music, -1)
  void Audio_StopMusic(void)              — calls Mix_HaltMusic()

Then replace every raw Mix_* call site in the rest of the code with the appropriate wrapper:
  Mix_HaltMusic()                      → Audio_StopMusic()
  Mix_PlayChannel(-1, sound_win, 0)    → Audio_PlaySound(app->sfxWin)
  Mix_PlayChannel(-1, sound_place_tile, 0) → Audio_PlaySound(app->sfxPlaceTile)
  Mix_PlayChannel(-1, sound_valid_word, 0) → Audio_PlaySound(app->sfxValidWord)
  Mix_PlayChannel(-1, sound_warning, 0)   → Audio_PlaySound(app->sfxWarning)
  Mix_PlayMusic(music_welcome, -1)     → Audio_PlayMusic(app->bgmWelcome)

The null check inside each wrapper prevents crashes if an asset failed to load.
No other logic should change.

Here is the code:
[paste the relevant functions here]
```

---

### Prompt 8 — Add null safety to the cleanup function

```
The current cleanup() function frees all SDL resources unconditionally, which will
crash if any resource failed to load during initialization:

  Mix_FreeChunk(sound_place_tile);   // unsafe if NULL
  TTF_CloseFont(font);               // unsafe if NULL
  SDL_DestroyRenderer(renderer);     // unsafe if NULL

Rewrite cleanup() (or split it into Audio_Cleanup() and Core_Cleanup()) so that
every resource is null-checked before being freed:

  if (handle) FreeFunction(handle);

The final cleanup order must be:
  1. SDL_StopTextInput()
  2. Free all Mix_Chunk handles (with null check)
  3. Free Mix_Music handle (with null check)
  4. Mix_Quit()
  5. Close TTF fonts (with null check)
  6. SDL_DestroyRenderer (with null check)
  7. SDL_DestroyWindow (with null check)
  8. TTF_Quit()
  9. SDL_Quit()

Do not change anything else.

Here is the cleanup function:
[paste cleanup() here]
```

---

### Prompt 9 — Mark internal functions and data as `static`

```
In the following C module, identify every function and variable that is only used
within this single file and not declared in any header (.h) file.
Add the static keyword to each of those definitions to enforce information hiding
and prevent accidental linkage from other translation units.

For logic.c, the following must become static:
  — char loadedDictionary[][] array
  — int totalLoadedWords variable
  — const char* specialBonusWords[] array
  — const int totalBonusWords variable
  — Logic_CompareStrings() function
  — Logic_IsWordAlreadyScored() function
  — Logic_CalculateWordScore() function
  — Logic_IsBonusWord() function

For graphics.c, mark as static:
  — Graphics_DrawText() function
  — Graphics_DrawTextCentered() function
  — Graphics_DrawButton() function

For input.c, mark as static:
  — Input_IsMouseOverButton() function

For main.c, mark as static:
  — Core_InitSDL() function
  — Core_InitGame() function
  — Core_Cleanup() function

Do not change any logic. Only add the static keyword to the listed items.

Here is the file:
[paste the relevant .c file here]
```

---

### Prompt 10 — Full end-to-end refactor (single master prompt)

> Use this if you want to perform the entire ver1 → ver2 transformation in one pass.

```
I have a monolithic C game file called game.c (632 lines). Refactor it completely
into a modular, professional architecture following all of these rules simultaneously:

FILE STRUCTURE
- Split into: src/main.c, src/logic.c, src/graphics.c, src/input.c, src/audio.c
- Create headers: include/config.h, include/types.h, include/logic.h,
  include/graphics.h, include/input.h, include/audio.h
- Use #ifndef include guards on all headers

GLOBAL VARIABLES
- Remove all 11 global variables
- Bundle SDL_Window*, SDL_Renderer*, both TTF_Font*, all Mix_Chunk* and Mix_Music*
  into a typedef struct AppContext in include/types.h
- Dictionary array and word count become static variables inside logic.c only

NAMING — Enums
- Remove GAME_ prefix: GAME_STATE_SPLASH → STATE_SPLASH (apply to all 6 values)

NAMING — Struct fields (snake_case → camelCase, booleans get "is" prefix)
- game_state→currentState, player_names→playerNames, tile_selected→isTileSelected,
  hovered→isHovered (apply the full rename list to all fields)

NAMING — Functions (flat → Module_Action namespaced)
- init_sdl→Core_InitSDL, load_dictionary→Logic_LoadDictionary,
  render_game→Graphics_RenderGame, handle_game_input→Input_HandleGame, etc.
  (apply the full rename map to all 22 functions)

NAMING — Internal data
- giant_dictionary→loadedDictionary, actual_word_count→totalLoadedWords,
  bonus_words→specialBonusWords, bonus_word_count→totalBonusWords

ALGORITHM — Word scanning loop
- Rewrite the compact for-loop condition in check_words_and_score() into a
  cleaner loop that resolves currentTile first, then uses an explicit break on '\0'

AUDIO — Wrapper API
- Create Audio_PlaySound(), Audio_PlayMusic(), Audio_StopMusic() in audio.c
- Replace all 8 raw Mix_* call sites with these wrappers
- Add null guard inside each wrapper

CLEANUP SAFETY
- Null-check every resource handle before calling its free/close/destroy function

STATIC SCOPE
- Mark all internal-only functions and data as static in their respective .c files
- Only functions declared in a .h header should have external linkage

OUTPUT: Provide all 11 files (5 .c + 6 .h) as separate clearly labeled code blocks.
Do not change any game logic, values, constants, or algorithms beyond what is listed.

Here is game.c:
[paste game.c here]
```

---

## 🧠 Design Principles Applied

| Principle | ver1 Status | ver2 Implementation |
|---|---|---|
| **Separation of Concerns** | ❌ All logic in one file | ✅ Rendering, logic, input, audio each in isolated files |
| **Dependency Injection** | ❌ Functions silently read globals | ✅ `AppContext*` passed explicitly to all functions |
| **Data Encapsulation** | ❌ Dictionary globally mutable | ✅ `static` in `logic.c` — invisible to all other modules |
| **Single Responsibility** | ❌ `game.c` does everything | ✅ Each `.c` file has exactly one clear purpose |
| **Simulated Namespacing** | ❌ Flat `snake_case` names | ✅ `Graphics_*`, `Audio_*`, `Input_*`, `Logic_*`, `Core_*` |
| **DRY** | ⚠️ Some constants inline | ✅ All magic numbers centralized in `config.h` |
| **Information Hiding** | ❌ All functions global linkage | ✅ 15 internal functions and 4 data vars marked `static` |
| **Null Safety** | ❌ Unconditional free in `cleanup()` | ✅ Every resource null-checked before free |
| **Layered Architecture** | ❌ No layers | ✅ UI/IO → Engine → Foundation; zero circular dependencies |

---

*WORDS COLLIDE — CSE 2100 Project Show · Student ID 240238 · February 2026*