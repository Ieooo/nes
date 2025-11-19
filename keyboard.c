#include "keyboard.h"
#include <stdio.h>
#include <stdlib.h>

static const Uint8* sdl_key_state = NULL;
/*
    fixed mapping now
    BUTTON_A       ==> L/l
    BUTTON_B       ==> K/k
    BUTTON_SELECT  ==> Shift
    BUTTON_START   ==> Space
    BUTTON_UP      ==> W/w
    BUTTON_DOWNA   ==> S/s
    BUTTON_LEFT    ==> A/a
    BUTTON_RIGHT   ==> D/d
*/
uint8_t kb_to_controller(uint32_t kb_value) {
    uint8_t controller_v = 0;
    switch (kb_value)
    {
    case 65: // A
        controller_v = BUTTON_LEFT;
        break;
    case 97: // a
        controller_v = BUTTON_LEFT;
        break;
    case 87: // W
        controller_v = BUTTON_UP;
        break;
    case 119: // w
        controller_v = BUTTON_UP;
        break;
    case 68: // D
        controller_v = BUTTON_RIGHT;
        break;
    case 100: // d
        controller_v = BUTTON_RIGHT;
        break;
    case 83: // S
        controller_v = BUTTON_DOWN;
        break;
    case 115: // s
        controller_v = BUTTON_DOWN;
        break;
    case 76: // L
        controller_v = BUTTON_A;
        break;
    case 108: // l
        controller_v = BUTTON_A;
        break;
    case 75: // K
        controller_v = BUTTON_B;
        break;
    case 107: // k
        controller_v = BUTTON_B;
        break;
    case 32: // Space
        controller_v = BUTTON_START;
        break;
    default:
        break;
    }
    return controller_v;
}

InputSystem* input_create(InputCallback cb, void *userdata)
{
    if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0) {
        return NULL;
    }

    InputSystem *in = malloc(sizeof *in);
    in->callback     = cb;
    in->userdata     = userdata;
    in->key_states   = SDL_GetKeyboardState(NULL);
    in->mouse_x = in->mouse_y = 0;
    in->mouse_buttons = 0;
    return in;
}

void input_poll(InputSystem *input)
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        InputEvent ie = {0};

        switch (ev.type) {
            case SDL_QUIT:
                // 主循环可自行处理
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                ie.type         = (ev.type == SDL_KEYDOWN) ? INPUT_KEY_DOWN : INPUT_KEY_UP;
                ie.key_scancode = ev.key.keysym.scancode;
                ie.key_repeat   = ev.key.repeat;
                ie.key_mod      = ev.key.keysym.mod;
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                ie.type         = (ev.type == SDL_MOUSEBUTTONDOWN) ? INPUT_MOUSE_DOWN : INPUT_MOUSE_UP;
                ie.mouse_button = ev.button.button;
                ie.mouse_x      = ev.button.x;
                ie.mouse_y      = ev.button.y;
                input->mouse_buttons = SDL_GetMouseState(NULL, NULL);
                break;

            case SDL_MOUSEMOTION:
                ie.type         = INPUT_MOUSE_MOTION;
                ie.mouse_x      = ev.motion.x;
                ie.mouse_y      = ev.motion.y;
                ie.mouse_rel_x  = ev.motion.xrel;
                ie.mouse_rel_y  = ev.motion.yrel;
                input->mouse_x  = ie.mouse_x;
                input->mouse_y  = ie.mouse_y;
                break;

            case SDL_MOUSEWHEEL:
                ie.type     = INPUT_MOUSE_WHEEL;
                ie.wheel_x  = ev.wheel.x;
                ie.wheel_y  = ev.wheel.y;
                break;

            default:
                continue;
        }

        if (input->callback) {
            input->callback(&ie, input->userdata);
        }
    }

    // 更新鼠标按钮状态（防止遗漏）
    input->mouse_buttons = SDL_GetMouseState(&input->mouse_x, &input->mouse_y);
}

void input_destroy(InputSystem *input)
{
    if (input) free(input);
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
}

// 查询
bool input_key_down(const InputSystem *input, SDL_Scancode code)
{
    return input->key_states && input->key_states[code];
}

bool input_mouse_down(const InputSystem *input, Uint8 button)
{
    return (input->mouse_buttons & SDL_BUTTON(button)) != 0;
}

void input_get_mouse_pos(const InputSystem *input, int *x, int *y)
{
    if (x) *x = input->mouse_x;
    if (y) *y = input->mouse_y;
}

