#ifndef RENDER_UTILS_H
#define RENDER_UTILS_H

#include <SDL3/SDL.h>
#include "math_utils.h"
#include "ui.h"

void draw_circle(SDL_Renderer* renderer, float cx, float cy, float radius, SDL_Color color);
void draw_thick_line(SDL_Renderer* renderer, Vec2 p1, Vec2 p2, float thickness, SDL_Color color);
void draw_digit(SDL_Renderer* r, int digit, float x, float y, float w, float h, SDL_Color color);
void draw_char(SDL_Renderer* r, char c, float x, float y, float w, float h, SDL_Color color);
void draw_string(SDL_Renderer* r, const char* str, float x, float y, float char_w, float char_h, SDL_Color color);
void draw_number(SDL_Renderer* r, float number, float x, float y, float char_w, float char_h, SDL_Color color, int decimals);

void draw_joint_label(SDL_Renderer* r, int joint_idx, float x, float y);
void draw_formula_box(SDL_Renderer* r);
void draw_button(SDL_Renderer* renderer, Button* b);
void draw_input(SDL_Renderer* r, InputBox* inp);

#endif // RENDER_UTILS_H
