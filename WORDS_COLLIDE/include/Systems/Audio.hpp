#ifndef AUDIO_HPP
#define AUDIO_HPP

#include "Core/Types.hpp"

class Audio {
public:
    explicit Audio(AppContext& ctx);
    ~Audio();

    bool init();
    void playSound(Mix_Chunk* sound);
    void playMusic(Mix_Music* music);
    void stopMusic();

private:
    AppContext& app;
    bool initialized = false;
};

#endif // AUDIO_HPP