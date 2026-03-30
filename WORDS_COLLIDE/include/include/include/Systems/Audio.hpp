#ifndef AUDIO_HPP
#define AUDIO_HPP

#include "Core/Types.hpp"
#include "Systems/IAudio.hpp"

class Audio : public IAudio {
public:
    explicit Audio(AppContext& ctx);
    ~Audio() override;

    bool init() override;
    void playSound(Mix_Chunk* sound) override;
    void playMusic(Mix_Music* music) override;
    void stopMusic() override;

private:
    AppContext& app;
    bool initialized = false;
};

#endif // AUDIO_HPP