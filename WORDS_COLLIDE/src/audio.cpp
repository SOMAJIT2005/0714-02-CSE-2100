#include "Audio.hpp"
#include <SDL2/SDL_mixer.h>
#include <iostream>

Audio::Audio(AppContext& ctx) : app(ctx) {}

bool Audio::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Mix_OpenAudio failed: " << Mix_GetError() << "\n";
        return false;
    }

    app.sfxPlaceTile = Mix_LoadWAV("assets/place.wav");
    app.sfxValidWord = Mix_LoadWAV("assets/success.wav");
    app.sfxWarning   = Mix_LoadWAV("assets/warning.wav");
    app.sfxWin       = Mix_LoadWAV("assets/win.wav");
    app.bgmWelcome   = Mix_LoadMUS("assets/welcome.mp3");

    initialized = true;
    return true;
}

void Audio::playSound(Mix_Chunk* sound) {
    if (sound) Mix_PlayChannel(-1, sound, 0);
}

void Audio::playMusic(Mix_Music* music) {
    if (music) Mix_PlayMusic(music, -1);
}

void Audio::stopMusic() {
    Mix_HaltMusic();
}

Audio::~Audio() {
    if (app.sfxPlaceTile) Mix_FreeChunk(app.sfxPlaceTile);
    if (app.sfxValidWord) Mix_FreeChunk(app.sfxValidWord);
    if (app.sfxWarning)   Mix_FreeChunk(app.sfxWarning);
    if (app.sfxWin)       Mix_FreeChunk(app.sfxWin);
    if (app.bgmWelcome)   Mix_FreeMusic(app.bgmWelcome);
    Mix_Quit();
}
