#include "video.h"

Video* video_create(int w, int h, const char* title)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return NULL;

    SDL_Window* win = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
    if (!win) return NULL;

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) { SDL_DestroyWindow(win); return NULL; }

    SDL_Texture* texture = SDL_CreateTexture(ren, 
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
    if (!texture) { SDL_DestroyWindow(win); return NULL; }

    Video* video = malloc(sizeof(Video));
    video->window = win;
    video->renderer = ren;
    video->texture = texture;
    video->width = w;
    video->height = h;
    return video;
}

void video_destroy(Video* video){
    if (!video) return;
    SDL_DestroyRenderer(video->renderer);
    SDL_DestroyWindow(video->window);
    SDL_Quit();
    free(video);
}

InputState video_poll_events(Video* video) {
    SDL_Event ev;
    InputState input = {0};

    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            input.quit = true;
        } else if (ev.type == SDL_MOUSEBUTTONDOWN) {
            input.mouse_down = true;
            input.mouse_x = ev.button.x;
            input.mouse_y = ev.button.y;
        } else if (ev.type == SDL_MOUSEBUTTONUP) {
            input.mouse_down = false;
        }
    }

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    input.mouse_x = mx;
    input.mouse_y = my;

    return input;
}

void video_render_line(Video* video, const uint32_t *buffer) {
    SDL_UpdateTexture(video->texture, NULL, buffer, video->width * sizeof(uint32_t));
    SDL_RenderClear(video->renderer);
    SDL_RenderCopy(video->renderer, video->texture, NULL, NULL);
    SDL_RenderPresent(video->renderer);
}

