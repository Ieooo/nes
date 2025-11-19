#ifndef __VIDEO_H__
#define __VIDEO_H__

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

typedef struct {
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Texture*  texture;

    int width, height;
} Video;

typedef struct {
    bool quit;
    bool mouse_down;
    int mouse_x, mouse_y;
} InputState;

Video* video_create(int, int, const char*);
InputState video_poll_events(Video*);
void video_destroy(Video*);
void video_render_line(Video*, const uint32_t*);

#endif