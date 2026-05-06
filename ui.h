#ifndef UI_H
#define UI_H

#include <SDL3/SDL.h>
#include <stdbool.h>

#define MAX_INPUT_LEN 16

typedef struct {
    SDL_FRect rect;
    char text[MAX_INPUT_LEN];
    int text_len;
    bool active;
    bool hovered;
    const char* label;
    const char* unit;
} InputBox;

typedef struct {
    SDL_FRect rect;
    const char* label;
    SDL_Color color;
    bool hovered;
    bool clicked;
} Button;

extern int window_width;
extern int window_height;
extern int sidebar_width;
extern int content_width;
extern int content_height;
extern bool show_formula;

// Defined in kinematics.h but needed for array size here
#ifndef MAX_SEGMENTS
#define MAX_SEGMENTS 6
#endif

extern InputBox inputs[MAX_SEGMENTS];
extern InputBox target_x_input;
extern InputBox target_y_input;

extern Button btn_add, btn_sub, btn_mode, btn_solve;

void update_layout(void);
void init_inputs(void);
void update_buttons_layout(void);
void sync_inputs_with_angles(void);

void update_button(Button* b, float mx, float my, bool mouse_pressed);
void update_input(InputBox* inp, float mx, float my, bool mouse_pressed);
void handle_text_input(InputBox* inp, const char* text);
void handle_backspace(InputBox* inp);
float get_input_value(InputBox* inp);

#endif // UI_H
