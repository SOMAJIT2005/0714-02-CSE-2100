#include "../include/audio.h"

bool Audio_Init(AppContext *app) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return false;
    
    app->sfxPlaceTile = Mix_LoadWAV("assets/place.wav");
    app->sfxValidWord = Mix_LoadWAV("assets/success.wav");
    app->sfxWarning   = Mix_LoadWAV("assets/warning.wav");
    app->sfxWin       = Mix_LoadWAV("assets/win.wav");
    app->bgmWelcome   = Mix_LoadMUS("assets/welcome.mp3"); 
    
    return true;
}

void Audio_PlaySound(Mix_Chunk *sound) {
    if(sound) Mix_PlayChannel(-1, sound, 0);
}

void Audio_PlayMusic(Mix_Music *music) {
    if(music) Mix_PlayMusic(music, -1);
}

void Audio_StopMusic(void) {
    Mix_HaltMusic();
}

void Audio_Cleanup(AppContext *app) {
    if(app->sfxPlaceTile) Mix_FreeChunk(app->sfxPlaceTile);
    if(app->sfxValidWord) Mix_FreeChunk(app->sfxValidWord);
    if(app->sfxWarning) Mix_FreeChunk(app->sfxWarning); 
    if(app->sfxWin) Mix_FreeChunk(app->sfxWin);
    if(app->bgmWelcome) Mix_FreeMusic(app->bgmWelcome);
    Mix_Quit();
}