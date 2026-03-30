
#ifndef I_AUDIO_HPP
#define I_AUDIO_HPP

#include <SDL2/SDL_mixer.h>

class IAudio {
public:
    virtual ~IAudio() = default;
    virtual bool init() = 0;
    virtual void playSound(Mix_Chunk* sound) = 0;
    virtual void playMusic(Mix_Music* music) = 0;
    virtual void stopMusic() = 0;
};

#endif // I_AUDIO_HPP