#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h> 


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define GRID_SIZE 20
#define TILE_SIZE (WINDOW_WIDTH / GRID_SIZE)
#define MAX_WORD_LENGTH 20
#define MAX_SCORED_WORDS 50
#define MAX_NAME_LENGTH 30 
#define GAME_DURATION_MS 600000    
#define WARNING_TIME_MS 50000      
#define WARNING_INTERVAL_MS 10000  
#define TURN_DURATION_MS 20000    
#define BONUS_WORD_POINTS 15       
#define MAX_BONUS_WORDS 50         
#define MAX_WORDS_IN_DICT 200000 


typedef enum {
    GAME_STATE_SPLASH,
    GAME_STATE_GET_NAMES,
    GAME_STATE_PLAYING,
    GAME_STATE_GAME_OVER,
    GAME_STATE_RESTART,
    GAME_STATE_QUIT
} GameStateType;

typedef struct {
    SDL_Rect rect;
    const char *text;
    bool hovered;
} Button;

typedef struct {
    GameStateType game_state;
    char player_names[2][MAX_NAME_LENGTH];
    int current_name_input; 
    Uint32 game_start_time; 
    Uint32 last_warning_time; 
    Uint32 turn_start_time; 
    
    char grid[GRID_SIZE][GRID_SIZE];
    int current_player;
    char current_letter;
    int selected_x, selected_y;
    bool tile_selected;
    int scores[2];
    char scored_words[MAX_SCORED_WORDS][MAX_WORD_LENGTH];
    int scored_word_count;

    Button give_up_button;
    Button play_again_button;
    Button start_game_button;
} GameState;


SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;
TTF_Font *font_large = NULL; 
Mix_Chunk *sound_place_tile = NULL;
Mix_Chunk *sound_valid_word = NULL;
Mix_Chunk *sound_warning = NULL;
Mix_Chunk *sound_win = NULL;
Mix_Music *music_welcome = NULL;
char giant_dictionary[MAX_WORDS_IN_DICT][MAX_WORD_LENGTH];
int actual_word_count = 0;

const char* bonus_words[MAX_BONUS_WORDS] = {
    "BONUS", "EXTRA", "SPECIAL", "MAGIC", "EYE", "GREEN"
};
int bonus_word_count = 6; 


bool init_sdl();
void load_dictionary();
void init_game(GameState *game);
void cleanup();
int compare_strings(const void *a, const void *b);
bool is_valid_word(const char *word);
bool is_word_already_scored(GameState *game, const char *word);
int calculate_word_score(const char *word);
int check_words_and_score(GameState *game, int x, int y, char letter);
bool is_mouse_over_button(SDL_Event *event, Button *button);
bool is_bonus_word(const char *word);
void render_text(TTF_Font *font_to_use, const char *text, int x, int y, SDL_Color color);
void render_text_centered(TTF_Font *font_to_use, const char *text, int y, SDL_Color color);
void render_button(Button *button);
void render_splash_screen(GameState *game);
void render_name_input(GameState *game);
void render_game(GameState *game);
void render_game_over(GameState *game);
void handle_splash_input(SDL_Event *event, GameState *game);
void handle_name_input(SDL_Event *event, GameState *game);
void handle_game_input(SDL_Event *event, GameState *game);
void handle_game_over_input(SDL_Event *event, GameState *game);


void load_dictionary() {
    FILE *file = fopen("assets/Dictionary.txt", "r");
    if (!file) {
        printf("FATAL ERROR: assets/Dictionary.txt not found!\n");
        return;
    }
    printf("Loading dictionary...\n");
    while (actual_word_count < MAX_WORDS_IN_DICT && 
           fscanf(file, "%s", giant_dictionary[actual_word_count]) == 1) 
    {
        for(int i = 0; giant_dictionary[actual_word_count][i]; i++){
            giant_dictionary[actual_word_count][i] = toupper(giant_dictionary[actual_word_count][i]);
        }
        actual_word_count++;
    }
    fclose(file);
    printf("Loaded %d words into memory.\n", actual_word_count);
}

bool init_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) return false;
    if (TTF_Init() == -1) return false;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return false;

    window = SDL_CreateWindow("WORDS_COLLIDE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
    if (!window) return false;
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) return false;
    
    font = TTF_OpenFont("assets/arial.ttf", 24);
    font_large = TTF_OpenFont("assets/arial.ttf", 48);
    sound_place_tile = Mix_LoadWAV("assets/place.wav");
    sound_valid_word = Mix_LoadWAV("assets/success.wav");
    sound_warning = Mix_LoadWAV("assets/warning.wav");
    sound_win = Mix_LoadWAV("assets/win.wav");
    music_welcome = Mix_LoadMUS("assets/welcome.mp3"); 
    
    return true;
}

void init_game(GameState *game) {
    game->game_state = GAME_STATE_SPLASH;
    game->current_name_input = 0;
    game->game_start_time = 0; 
    game->last_warning_time = 0; 
    game->turn_start_time = 0;
    memset(game->player_names[0], 0, MAX_NAME_LENGTH);
    memset(game->player_names[1], 0, MAX_NAME_LENGTH);
    memset(game->grid, 0, sizeof(game->grid));
    game->current_player = 0;
    game->current_letter = '\0';
    game->tile_selected = false;
    game->selected_x = -1;
    game->selected_y = -1;
    game->scores[0] = 0;
    game->scores[1] = 0;
    game->scored_word_count = 0;
    memset(game->scored_words, 0, sizeof(game->scored_words));
    
    game->give_up_button.rect = (SDL_Rect){ WINDOW_WIDTH - 130, WINDOW_HEIGHT - 45, 120, 35 };
    game->give_up_button.text = "Give Up";
    game->give_up_button.hovered = false;

    game->play_again_button.rect = (SDL_Rect){ (WINDOW_WIDTH - 150) / 2, 480, 150, 40 };
    game->play_again_button.text = "Play Again";
    game->play_again_button.hovered = false;

    game->start_game_button.rect = (SDL_Rect){ (WINDOW_WIDTH - 200) / 2, 400, 200, 50 };
    game->start_game_button.text = "Start Game";
    game->start_game_button.hovered = false;

    Mix_PlayMusic(music_welcome, -1);
}

void cleanup() {
    SDL_StopTextInput();
    Mix_FreeChunk(sound_place_tile);
    Mix_FreeChunk(sound_valid_word);
    Mix_FreeChunk(sound_warning); 
    Mix_FreeChunk(sound_win);
    Mix_FreeMusic(music_welcome);
    TTF_CloseFont(font);
    TTF_CloseFont(font_large); 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}


int compare_strings(const void *a, const void *b) {
    return strcmp((const char *)a, (const char *)b);
}

bool is_valid_word(const char *word) {
    char temp_word[MAX_WORD_LENGTH];
    strcpy(temp_word, word);
    for(int i = 0; temp_word[i]; i++){
      temp_word[i] = toupper(temp_word[i]);
    }
    void *result = bsearch(temp_word, giant_dictionary, actual_word_count, MAX_WORD_LENGTH, compare_strings); 
    return (result != NULL);
}

bool is_word_already_scored(GameState *game, const char *word) {
    for (int i = 0; i < game->scored_word_count; i++) {
        if (strcasecmp(word, game->scored_words[i]) == 0) return true;
    }
    return false;
}

int calculate_word_score(const char *word) {
    int length = 0;
    for (int i = 0; word[i]; i++) {
        if (isalpha(word[i])) length++;
    }
    return length;
}

bool is_bonus_word(const char *word) {
    char upper_word[MAX_WORD_LENGTH];
    strcpy(upper_word, word);
    for(int i = 0; upper_word[i]; i++) {
      upper_word[i] = toupper(upper_word[i]);
    }
    for (int i = 0; i < bonus_word_count; i++) {
        if (strcmp(upper_word, bonus_words[i]) == 0) return true; 
    }
    return false;
}

int check_words_and_score(GameState *game, int x, int y, char letter) {
    int total_score = 0;
    
    // Horizontal Check
    char h_word[MAX_WORD_LENGTH] = "";
    int h_start = x;
    while (h_start > 0 && game->grid[h_start - 1][y] != '\0') h_start--;
    int h_len = 0;
    for (int i = h_start; i < GRID_SIZE && (game->grid[i][y] != '\0' || i == x); i++) {
        if (h_len >= MAX_WORD_LENGTH - 1) break;
        h_word[h_len++] = (i == x) ? letter : game->grid[i][y];
    }
    h_word[h_len] = '\0';
    if (h_len > 1 && is_valid_word(h_word) && !is_word_already_scored(game, h_word)) {
        int score = calculate_word_score(h_word);
        if (is_bonus_word(h_word)) score += BONUS_WORD_POINTS;
        total_score += score;
        if (game->scored_word_count < MAX_SCORED_WORDS) strcpy(game->scored_words[game->scored_word_count++], h_word);
    }

    // Vertical Check (Top-to-Bottom)
    char v_word[MAX_WORD_LENGTH] = "";
    int v_start = y;
    while (v_start > 0 && game->grid[x][v_start - 1] != '\0') v_start--;
    int v_len = 0;
    for (int i = v_start; i < GRID_SIZE && (game->grid[x][i] != '\0' || i == y); i++) {
        if (v_len >= MAX_WORD_LENGTH - 1) break;
        v_word[v_len++] = (i == y) ? letter : game->grid[x][i];
    }
    v_word[v_len] = '\0';
    if (v_len > 1 && is_valid_word(v_word) && !is_word_already_scored(game, v_word)) {
        int score = calculate_word_score(v_word);
        if (is_bonus_word(v_word)) score += BONUS_WORD_POINTS;
        total_score += score;
        if (game->scored_word_count < MAX_SCORED_WORDS) strcpy(game->scored_words[game->scored_word_count++], v_word);
    }
    
    return total_score;
}

bool is_mouse_over_button(SDL_Event *event, Button *button) {
    if (event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONDOWN) {
        int x = event->motion.x;
        int y = event->motion.y;
        return (x >= button->rect.x && x <= button->rect.x + button->rect.w &&
                y >= button->rect.y && y <= button->rect.y + button->rect.h);
    }
    return false;
}


void render_text(TTF_Font *font_to_use, const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font_to_use, text, color);
    if (surface) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }
}

void render_text_centered(TTF_Font *font_to_use, const char *text, int y, SDL_Color color) {
    int text_width = 0, text_height = 0;
    TTF_SizeText(font_to_use, text, &text_width, &text_height);
    int x = (WINDOW_WIDTH - text_width) / 2;
    render_text(font_to_use, text, x, y, color);
}

void render_button(Button *button) {
    SDL_Color button_color = {100, 50, 150, 255}; 
    SDL_Color hover_color = {160, 130, 200, 255}; 
    SDL_Color text_color = {255, 255, 255, 255}; 

    if (button->hovered) SDL_SetRenderDrawColor(renderer, hover_color.r, hover_color.g, hover_color.b, hover_color.a);
    else SDL_SetRenderDrawColor(renderer, button_color.r, button_color.g, button_color.b, button_color.a);
    
    SDL_RenderFillRect(renderer, &button->rect);

    int text_width = 0, text_height = 0;
    TTF_SizeText(font, button->text, &text_width, &text_height);
    int text_x = button->rect.x + (button->rect.w - text_width) / 2;
    int text_y = button->rect.y + (button->rect.h - text_height) / 2;
    render_text(font, button->text, text_x, text_y, text_color);
}

void render_splash_screen(GameState *game) {
    SDL_SetRenderDrawColor(renderer, 230, 210, 255, 255);
    SDL_RenderClear(renderer);
    SDL_Color title_color = {100, 50, 150, 255}; 
    render_text_centered(font_large, "WORDS_COLLIDE", 250, title_color);
    render_button(&game->start_game_button);
    SDL_RenderPresent(renderer);
}

void render_name_input(GameState *game) {
    SDL_SetRenderDrawColor(renderer, 230, 210, 255, 255);
    SDL_RenderClear(renderer);
    SDL_Color title_color = {100, 50, 150, 255}; 
    SDL_Color box_color = {160, 130, 200, 255};   
    SDL_Color text_color = {0, 0, 0, 255};         
    SDL_Color prompt_color = {50, 50, 50, 255};  
    
    render_text_centered(font_large, "Enter Player Names", 150, title_color);
    char prompt[50];
    sprintf(prompt, "Enter Player %d Name:", game->current_name_input + 1);
    render_text_centered(font, prompt, 250, prompt_color);
    
    SDL_Rect input_box_rect = { 100, 300, WINDOW_WIDTH - 200, 50 };
    SDL_SetRenderDrawColor(renderer, box_color.r, box_color.g, box_color.b, box_color.a);
    SDL_RenderDrawRect(renderer, &input_box_rect);
    
    const char *current_name = game->player_names[game->current_name_input];
    int text_width = 0, text_height = 0;
    if (strlen(current_name) > 0) {
        TTF_SizeText(font, current_name, &text_width, &text_height);
        render_text(font, current_name, input_box_rect.x + 10, input_box_rect.y + 10, text_color);
    }
    if ((SDL_GetTicks() / 500) % 2 == 0) {
        SDL_Rect cursor_rect = { input_box_rect.x + 10 + text_width + 2, input_box_rect.y + 8, 3, 34 };
        SDL_SetRenderDrawColor(renderer, text_color.r, text_color.g, text_color.b, text_color.a);
        SDL_RenderFillRect(renderer, &cursor_rect);
    }
    render_text_centered(font, "Press ENTER to confirm", 400, prompt_color);
    SDL_RenderPresent(renderer);
}

void render_game(GameState *game) {
    SDL_SetRenderDrawColor(renderer, 230, 210, 255, 255);
    SDL_RenderClear(renderer);
    
    SDL_Color timer_color = {100, 50, 150, 255};
    Uint32 elapsed_time_ms = SDL_GetTicks() - game->game_start_time;
    Uint32 time_left_ms = GAME_DURATION_MS - elapsed_time_ms;
    if (elapsed_time_ms >= GAME_DURATION_MS) time_left_ms = 0;
    
    int total_seconds = time_left_ms / 1000;
    char timer_text[20];
    sprintf(timer_text, "Time: %02d:%02d", total_seconds / 60, total_seconds % 60);
    if (time_left_ms < WARNING_TIME_MS && time_left_ms > 0 && (SDL_GetTicks() / 250) % 2 == 0) { 
        timer_color = (SDL_Color){255, 0, 0, 255};
    }
    
    int text_width = 0, text_height = 0;
    TTF_SizeText(font, timer_text, &text_width, &text_height);
    render_text(font, timer_text, WINDOW_WIDTH - text_width - 10, 10, timer_color);

    // --- Turn Timer Visual Bar ---
    Uint32 turn_elapsed = SDL_GetTicks() - game->turn_start_time;
    float ratio = 1.0f - ((float)turn_elapsed / TURN_DURATION_MS);
    if (ratio < 0) ratio = 0;

    SDL_Rect bar_bg = { 10, 10, 200, 15 };
    SDL_Rect bar_fg = { 10, 10, (int)(200 * ratio), 15 };
    
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255); // Red Background
    SDL_RenderFillRect(renderer, &bar_bg);
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255); // Green Foreground
    SDL_RenderFillRect(renderer, &bar_fg);
    render_text(font, "Turn Time", 10, 30, (SDL_Color){50, 50, 50, 255});

    // --- Grid Rendering ---
    SDL_SetRenderDrawColor(renderer, 160, 130, 200, 255);
    for (int i = 0; i <= GRID_SIZE; i++) {
        SDL_RenderDrawLine(renderer, i * TILE_SIZE, 0, i * TILE_SIZE, WINDOW_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, i * TILE_SIZE, WINDOW_WIDTH, i * TILE_SIZE);
    }
    
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (game->grid[i][j]) {
                SDL_Color color = {0, 0, 0, 255};
                SDL_Surface *surface = TTF_RenderText_Solid(font, (char[]){game->grid[i][j], '\0'}, color);
                if (surface) {
                    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_Rect rect = {i * TILE_SIZE + 10, j * TILE_SIZE + 10, TILE_SIZE - 20, TILE_SIZE - 20};
                    SDL_RenderCopy(renderer, texture, NULL, &rect);
                    SDL_DestroyTexture(texture);
                    SDL_FreeSurface(surface);
                }
            }
        }
    }
    
    if (game->tile_selected) {
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 100);
        SDL_Rect rect = {game->selected_x * TILE_SIZE, game->selected_y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        SDL_RenderFillRect(renderer, &rect);
        
        if (game->current_letter) {
            SDL_Surface *surface = TTF_RenderText_Solid(font, (char[]){game->current_letter, '\0'}, (SDL_Color){0, 0, 0, 255});
            if (surface) {
                SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect text_rect = {game->selected_x * TILE_SIZE + 10, game->selected_y * TILE_SIZE + 10, TILE_SIZE - 20, TILE_SIZE - 20};
                SDL_RenderCopy(renderer, texture, NULL, &text_rect);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
            }
        }
    }
    
    char score_text[200];
    const char *p1_name = (strlen(game->player_names[0]) > 0) ? game->player_names[0] : "Player 1";
    const char *p2_name = (strlen(game->player_names[1]) > 0) ? game->player_names[1] : "Player 2";
    const char *turn_name = (game->current_player == 0) ? p1_name : p2_name;
    sprintf(score_text, "%s: %d  |  %s: %d  |  Turn: %s", p1_name, game->scores[0], p2_name, game->scores[1], turn_name);
    
    render_text(font, score_text, 10, WINDOW_HEIGHT - 40, (SDL_Color){0, 0, 0, 255});
    render_button(&game->give_up_button);
    SDL_RenderPresent(renderer);
}

void render_game_over(GameState *game) {
    SDL_SetRenderDrawColor(renderer, 230, 210, 255, 255);
    SDL_RenderClear(renderer);
    
    Uint32 elapsed_time_ms = SDL_GetTicks() - game->game_start_time;
    if (elapsed_time_ms >= GAME_DURATION_MS) render_text_centered(font_large, "Time's Up!", 150, (SDL_Color){100, 50, 150, 255});
    else render_text_centered(font_large, "Game Over", 150, (SDL_Color){100, 50, 150, 255});
    
    const char *p1_name = (strlen(game->player_names[0]) > 0) ? game->player_names[0] : "Player 1";
    const char *p2_name = (strlen(game->player_names[1]) > 0) ? game->player_names[1] : "Player 2";
    
    char p1_score_text[100];
    char p2_score_text[100];
    sprintf(p1_score_text, "%s's Score: %d", p1_name, game->scores[0]);
    sprintf(p2_score_text, "%s's Score: %d", p2_name, game->scores[1]);
    
    render_text_centered(font, p1_score_text, 250, (SDL_Color){0, 0, 0, 255});
    render_text_centered(font, p2_score_text, 300, (SDL_Color){0, 0, 0, 255});
    
    char winner_text[100];
    if (game->scores[0] > game->scores[1]) sprintf(winner_text, "%s wins!", p1_name);
    else if (game->scores[1] > game->scores[0]) sprintf(winner_text, "%s wins!", p2_name);
    else sprintf(winner_text, "It's a tie!");
    
    render_text_centered(font_large, winner_text, 400, (SDL_Color){0, 100, 0, 255});
    render_button(&game->play_again_button);
    render_text_centered(font, "Press any key to exit", 550, (SDL_Color){50, 50, 50, 255});
    SDL_RenderPresent(renderer);
}


void handle_splash_input(SDL_Event *event, GameState *game) {
    if (event->type == SDL_QUIT) game->game_state = GAME_STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION) game->start_game_button.hovered = is_mouse_over_button(event, &game->start_game_button);
    else if (event->type == SDL_MOUSEBUTTONDOWN && is_mouse_over_button(event, &game->start_game_button)) {
        game->game_state = GAME_STATE_GET_NAMES;
        SDL_StartTextInput(); 
    }
}

void handle_name_input(SDL_Event *event, GameState *game) {
    if (event->type == SDL_QUIT) game->game_state = GAME_STATE_QUIT;
    else if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_BACKSPACE) {
            int len = strlen(game->player_names[game->current_name_input]);
            if (len > 0) game->player_names[game->current_name_input][len - 1] = '\0';
        }
        else if (event->key.keysym.sym == SDLK_RETURN) {
            if (strlen(game->player_names[game->current_name_input]) > 0) {
                if (game->current_name_input == 0) game->current_name_input = 1; 
                else {
                    game->game_state = GAME_STATE_PLAYING;
                    game->game_start_time = SDL_GetTicks(); 
                    game->turn_start_time = SDL_GetTicks(); 
                    SDL_StopTextInput(); 
                    Mix_HaltMusic(); 
                }
            }
        }
    } else if (event->type == SDL_TEXTINPUT) {
        if (strlen(game->player_names[game->current_name_input]) < MAX_NAME_LENGTH - 1) {
            if (isalpha(event->text.text[0]) || isdigit(event->text.text[0]) || isspace(event->text.text[0])) {
                 strcat(game->player_names[game->current_name_input], event->text.text);
            }
        }
    }
}

void handle_game_input(SDL_Event *event, GameState *game) {
    if (event->type == SDL_QUIT) game->game_state = GAME_STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION) game->give_up_button.hovered = is_mouse_over_button(event, &game->give_up_button);
    else if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (is_mouse_over_button(event, &game->give_up_button)) {
            Mix_HaltMusic(); 
            Mix_PlayChannel(-1, sound_win, 0); 
            game->game_state = GAME_STATE_GAME_OVER;
            SDL_StopTextInput();
            return;
        }
        int x = event->button.x / TILE_SIZE;
        int y = event->button.y / TILE_SIZE;
        if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE && !game->grid[x][y]) {
            game->tile_selected = true;
            game->selected_x = x;
            game->selected_y = y;
            SDL_StartTextInput(); 
        }
    } else if (event->type == SDL_KEYDOWN) {
        if (game->tile_selected && event->key.keysym.sym == SDLK_RETURN && game->current_letter) {
            game->grid[game->selected_x][game->selected_y] = game->current_letter;
            Mix_PlayChannel(-1, sound_place_tile, 0);
            int score = check_words_and_score(game, game->selected_x, game->selected_y, game->current_letter);
            if (score > 0) {
                Mix_PlayChannel(-1, sound_valid_word, 0);
                game->scores[game->current_player] += score;
            }
            game->current_player = (game->current_player + 1) % 2;
            game->tile_selected = false;
            game->current_letter = '\0';
            game->turn_start_time = SDL_GetTicks(); 
            SDL_StopTextInput(); 
        }
    } else if (event->type == SDL_TEXTINPUT) {
        if (game->tile_selected && strlen(event->text.text) == 1 && isalpha(event->text.text[0])) {
            game->current_letter = toupper(event->text.text[0]);
        }
    }
}

void handle_game_over_input(SDL_Event *event, GameState *game) {
    if (event->type == SDL_QUIT) game->game_state = GAME_STATE_QUIT;
    else if (event->type == SDL_MOUSEMOTION) game->play_again_button.hovered = is_mouse_over_button(event, &game->play_again_button);
    else if (event->type == SDL_MOUSEBUTTONDOWN && is_mouse_over_button(event, &game->play_again_button)) game->game_state = GAME_STATE_RESTART;
    else if (event->type == SDL_KEYDOWN) game->game_state = GAME_STATE_QUIT;
}

int main(int argc, char *argv[]) {
    load_dictionary();
    if (actual_word_count == 0) return 1; 
    if (!init_sdl()) return 1;

    GameState game;
    init_game(&game); 
    SDL_Event event;
    
    while (game.game_state != GAME_STATE_QUIT) {
        if (game.game_state == GAME_STATE_RESTART) {
            init_game(&game); 
            continue; 
        }
        
        if (game.game_state == GAME_STATE_PLAYING) {
            Uint32 current_time = SDL_GetTicks();
            Uint32 elapsed_ms = current_time - game.game_start_time;
            if (elapsed_ms >= GAME_DURATION_MS) {
                Mix_HaltMusic(); 
                Mix_PlayChannel(-1, sound_win, 0); 
                game.game_state = GAME_STATE_GAME_OVER;
                SDL_StopTextInput(); 
            }
            
            Uint32 remaining_ms = GAME_DURATION_MS - elapsed_ms;
            if (remaining_ms < WARNING_TIME_MS) {
                if (current_time - game.last_warning_time > WARNING_INTERVAL_MS) {
                    Mix_PlayChannel(-1, sound_warning, 0); 
                    game.last_warning_time = current_time; 
                }
            }

            if (current_time - game.turn_start_time >= TURN_DURATION_MS) {
                game.current_player = (game.current_player + 1) % 2;
                game.tile_selected = false;
                game.current_letter = '\0';
                game.turn_start_time = SDL_GetTicks();
                SDL_StopTextInput();
            }
        }

        while (SDL_PollEvent(&event)) {
            switch (game.game_state) {
                case GAME_STATE_SPLASH: handle_splash_input(&event, &game); break;
                case GAME_STATE_GET_NAMES: handle_name_input(&event, &game); break;
                case GAME_STATE_PLAYING: handle_game_input(&event, &game); break;
                case GAME_STATE_GAME_OVER: handle_game_over_input(&event, &game); break;
                default: break; 
            }
        }

        SDL_RaiseWindow(window);
        switch (game.game_state) {
            case GAME_STATE_SPLASH: render_splash_screen(&game); break;
            case GAME_STATE_GET_NAMES: render_name_input(&game); break;
            case GAME_STATE_PLAYING: render_game(&game); break;
            case GAME_STATE_GAME_OVER: render_game_over(&game); break;
            default: break;
        }
        SDL_Delay(10);
    }
    cleanup();
    return 0;
}