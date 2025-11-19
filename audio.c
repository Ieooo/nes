#include "audio.h"


static void audio_callback(void* userdata, uint8_t* stream, int len) {
    Audio* a = (Audio*)userdata;
    float* fstream = (float*)stream;
    int samples = len / sizeof(float);

    for (int i = 0; i < samples; ++i) {
        float sample = 0.0f;
        for (size_t c = 0; c < a->current.count; ++c) {
            const AudioCommand* cmd = &a->current.commands[c];
            if (!cmd->play) continue;

            float t = a->phase + (i / 48000.0f);
            float amp = cmd->volume * sinf(2 * M_PI * cmd->frequency * t);
            sample += amp;
        }

        fstream[i] = sample;
        a->phase += 1.0f / 48000.0f;
    }
}

Audio* audio_create() {
    if (SDL_Init(SDL_INIT_AUDIO) != 0) return NULL;

    Audio* a = malloc(sizeof(Audio));
    SDL_AudioSpec want = {0}, have;
    want.freq = 48000;
    want.format = AUDIO_F32;
    want.channels = 1;
    want.samples = 512;
    want.callback = audio_callback;
    want.userdata = a;

    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (dev == 0) return NULL;

    a->dev = dev;
    a->current.commands = NULL;
    a->current.count = 0;
    a->phase = 0;

    SDL_PauseAudioDevice(dev, 0);
    return a;
}

void audio_destroy(Audio* audio) {
    if (!audio) return;
    SDL_CloseAudioDevice(audio->dev);
    if (audio->current.commands) free(audio->current.commands);
    free(audio);
}

void audio_update(Audio* audio, const AudioSnapshot* snapshot)
{
    if (audio->current.commands) {
        free(audio->current.commands);
    }
    audio->current.commands = malloc(snapshot->count * sizeof(AudioCommand));
    memcpy(audio->current.commands, snapshot->commands, snapshot->count * sizeof(AudioCommand));
    audio->current.count = snapshot->count;
}