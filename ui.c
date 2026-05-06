#include "ui.h"
#include "kinematics.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int window_width = 1280;
int window_height = 720;
int sidebar_width = 320;
int content_width;
int content_height;
bool show_formula = true;

InputBox inputs[MAX_SEGMENTS];
InputBox target_x_input;
InputBox target_y_input;

Button btn_add, btn_sub, btn_mode, btn_solve;

void update_layout() {
    content_width = window_width - sidebar_width;
    content_height = window_height;
    base_pos = (Vec2){ content_width / 2.0f, content_height - 80.0f };
}

void init_inputs() {
    for (int i = 0; i < MAX_SEGMENTS; i++) {
        inputs[i].rect = (SDL_FRect){window_width - sidebar_width + 20, 280.0f + i * 50, 100, 32};
        inputs[i].text[0] = '0';
        inputs[i].text[1] = '\0';
        inputs[i].text_len = 1;
        inputs[i].active = false;
        inputs[i].hovered = false;
        inputs[i].label = "Joint";
        inputs[i].unit = "°";
    }

    target_x_input = (InputBox){{window_width - sidebar_width + 20, 200, 120, 32}, "400", 3, false, false, "Target X", "px"};
    target_y_input = (InputBox){{window_width - sidebar_width + 160, 200, 120, 32}, "300", 3, false, false, "Target Y", "px"};
}

void sync_inputs_with_angles() {
    for (int i = 0; i < num_segments; i++) {
        snprintf(inputs[i].text, MAX_INPUT_LEN, "%.1f", joint_angles[i]);
        inputs[i].text_len = strlen(inputs[i].text);
    }
}

void update_buttons_layout() {
    int sx = window_width - sidebar_width + 20;
    btn_add.rect = (SDL_FRect){sx, 60, 130, 36};
    btn_sub.rect = (SDL_FRect){sx + 150, 60, 130, 36};
    btn_mode.rect = (SDL_FRect){sx, 110, 280, 36};
    btn_solve.rect = (SDL_FRect){sx, 160, 280, 36};

    btn_add.label = "+ Add Joint";
    btn_sub.label = "- Rem Joint";
    btn_mode.label = current_mode == MODE_IK ? "Switch to FK Mode" : "Switch to IK Mode";
    btn_solve.label = current_mode == MODE_IK ? "Solve IK" : "Apply FK";

    btn_add.color = (SDL_Color){45, 120, 65, 255};
    btn_sub.color = (SDL_Color){160, 50, 50, 255};
    btn_mode.color = (SDL_Color){60, 80, 140, 255};
    btn_solve.color = (SDL_Color){140, 100, 40, 255};

    for (int i = 0; i < num_segments; i++) {
        inputs[i].rect.x = sx;
        inputs[i].rect.y = 255.0f + i * 55;
    }

    target_x_input.rect.x = sx;
    target_x_input.rect.y = 255;
    target_y_input.rect.x = sx + 150;
    target_y_input.rect.y = 255;
}

void update_button(Button* b, float mx, float my, bool mouse_pressed) {
    b->hovered = (mx >= b->rect.x && mx <= b->rect.x + b->rect.w &&
                  my >= b->rect.y && my <= b->rect.y + b->rect.h);
    b->clicked = (b->hovered && mouse_pressed);
}

void update_input(InputBox* inp, float mx, float my, bool mouse_pressed) {
    inp->hovered = (mx >= inp->rect.x && mx <= inp->rect.x + inp->rect.w &&
                    my >= inp->rect.y && my <= inp->rect.y + inp->rect.h);
    if (mouse_pressed && inp->hovered) {
        inp->active = true;
    } else if (mouse_pressed && !inp->hovered) {
        inp->active = false;
    }
}

void handle_text_input(InputBox* inp, const char* text) {
    if (!inp->active) return;
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        if ((c >= '0' && c <= '9') || c == '.' || c == '-') {
            if (inp->text_len < MAX_INPUT_LEN - 1) {
                inp->text[inp->text_len++] = c;
                inp->text[inp->text_len] = '\0';
            }
        }
    }
}

void handle_backspace(InputBox* inp) {
    if (!inp->active || inp->text_len <= 0) return;
    inp->text[--inp->text_len] = '\0';
    if (inp->text_len == 0) {
        inp->text[0] = '0';
        inp->text[1] = '\0';
        inp->text_len = 1;
    }
}

float get_input_value(InputBox* inp) {
    return atof(inp->text);
}
