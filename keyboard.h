#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <stdint.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#define BUTTON_A        0b10000000
#define BUTTON_B        0b01000000  
#define BUTTON_SELECT   0b00100000
#define BUTTON_START    0b00010000
#define BUTTON_UP       0b00001000
#define BUTTON_DOWN     0b00000100
#define BUTTON_LEFT     0b00000010
#define BUTTON_RIGHT    0b00000001

typedef enum {
    INPUT_KEY_DOWN,
    INPUT_KEY_UP,
    INPUT_MOUSE_DOWN,
    INPUT_MOUSE_UP,
    INPUT_MOUSE_MOTION,
    INPUT_MOUSE_WHEEL
} InputEventType;

typedef struct {
    InputEventType type;

    // 键盘
    SDL_Scancode key_scancode;
    bool         key_repeat;
    Uint16       key_mod;     // KMOD_*

    // 鼠标按钮
    Uint8        mouse_button;   // SDL_BUTTON_LEFT 等
    int          mouse_x, mouse_y;

    // 鼠标移动
    int          mouse_rel_x, mouse_rel_y;

    // 鼠标滚轮
    int          wheel_x, wheel_y;
} InputEvent;

// 回调（可选）
typedef void (*InputCallback)(const InputEvent *ev, void *userdata);

// 输入组件
typedef struct {
    InputCallback callback;
    void         *userdata;
    const Uint8  *key_states;     // SDL_GetKeyboardState()
    int           mouse_x, mouse_y;
    Uint8         mouse_buttons;
} InputSystem;

uint8_t kb_to_controller(uint32_t);
// 初始化（只需调用一次）
InputSystem* input_create(InputCallback cb, void *userdata);

// 每帧调用（非阻塞）
void input_poll(InputSystem *input);

// 销毁
void input_destroy(InputSystem *input);

// 查询函数（实时状态）
bool input_key_down(const InputSystem *input, SDL_Scancode code);
bool input_mouse_down(const InputSystem *input, Uint8 button);
void input_get_mouse_pos(const InputSystem *input, int *x, int *y);

#endif