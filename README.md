# 🔤 WORDS COLLIDE

> **A 2-Player Real-Time Word Formation Board Game — Built in C with SDL2**
>
> **Course:** CSE 2100 Project Show &nbsp;|&nbsp; **Student:** 0714-02 &nbsp;|&nbsp; **Date:** February 2026

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
10. [Design Principles Applied](#-design-principles-applied)

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

*WORDS COLLIDE — CSE 2100 Project Show · Student ID 0714-02 · February 2026*