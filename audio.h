#ifndef __AUDIO_H__
#define __AUDIO_H__


#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE     44100
#define FREQUENCY       440.0
#define AMPLITUDE       0.2     // 避免削波
#define DURATION        3       // 秒

#include <SDL2/SDL_audio.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    float frequency;   // Hz
    float duration;    // 秒
    float volume;      // 0.0 ~ 1.0
    bool play;
} AudioCommand;

typedef struct {
    AudioCommand* commands;
    size_t count;
} AudioSnapshot;

typedef struct {
    SDL_AudioDeviceID dev;
    AudioSnapshot current;
    float phase;
} Audio;

Audio* audio_create(void);
void audio_destroy(Audio* audio);
void audio_update(Audio* audio, const AudioSnapshot* snapshot);


#endif