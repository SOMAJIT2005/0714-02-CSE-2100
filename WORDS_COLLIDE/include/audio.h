#ifndef AUDIO_H
#define AUDIO_H

#include "types.h"

bool Audio_Init(AppContext *app);
void Audio_Cleanup(AppContext *app);
void Audio_PlaySound(Mix_Chunk *sound);
void Audio_PlayMusic(Mix_Music *music);
void Audio_StopMusic(void);

#endif // AUDIO_H