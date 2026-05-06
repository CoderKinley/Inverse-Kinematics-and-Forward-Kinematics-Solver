#include "render_utils.h"
#include "kinematics.h"
#include <stdio.h>

void draw_circle(SDL_Renderer* renderer, float cx, float cy, float radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderPoint(renderer, cx + dx, cy + dy);
            }
        }
    }
}

void draw_thick_line(SDL_Renderer* renderer, Vec2 p1, Vec2 p2, float thickness, SDL_Color color) {
    Vec2 dir = sub(p2, p1);
    float l = length(dir);
    if (l == 0) return;
    dir = (Vec2){dir.x / l, dir.y / l};
    Vec2 normal = {-dir.y, dir.x};
    Vec2 offset = mul(normal, thickness / 2.0f);

    SDL_Vertex vertices[4];
    for (int i = 0; i < 4; i++) {
        vertices[i].color.r = color.r / 255.0f;
        vertices[i].color.g = color.g / 255.0f;
        vertices[i].color.b = color.b / 255.0f;
        vertices[i].color.a = color.a / 255.0f;
    }

    Vec2 v0 = add(p1, offset);
    Vec2 v1 = sub(p1, offset);
    Vec2 v2 = add(p2, offset);
    Vec2 v3 = sub(p2, offset);

    vertices[0].position.x = v0.x; vertices[0].position.y = v0.y;
    vertices[1].position.x = v1.x; vertices[1].position.y = v1.y;
    vertices[2].position.x = v2.x; vertices[2].position.y = v2.y;
    vertices[3].position.x = v3.x; vertices[3].position.y = v3.y;

    int indices[6] = {0, 1, 2, 1, 3, 2};
    SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);
}

void draw_digit(SDL_Renderer* r, int digit, float x, float y, float w, float h, SDL_Color color) {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    bool segments[10][7] = {
        {1,1,1,0,1,1,1}, {0,0,1,0,0,1,0}, {1,0,1,1,1,0,1}, {1,0,1,1,0,1,1},
        {0,1,1,1,0,1,0}, {1,1,0,1,0,1,1}, {1,1,0,1,1,1,1}, {1,0,1,0,0,1,0},
        {1,1,1,1,1,1,1}, {1,1,1,1,0,1,1}
    };
    if (digit < 0 || digit > 9) return;
    bool* seg = segments[digit];
    float hh = h / 2.0f;
    
    if (seg[0]) SDL_RenderLine(r, x+1, y, x+w-1, y);
    if (seg[1]) SDL_RenderLine(r, x, y+1, x, y+hh-1);
    if (seg[2]) SDL_RenderLine(r, x+w, y+1, x+w, y+hh-1);
    if (seg[3]) SDL_RenderLine(r, x+1, y+hh, x+w-1, y+hh);
    if (seg[4]) SDL_RenderLine(r, x, y+hh+1, x, y+h-1);
    if (seg[5]) SDL_RenderLine(r, x+w, y+hh+1, x+w, y+h-1);
    if (seg[6]) SDL_RenderLine(r, x+1, y+h, x+w-1, y+h);
}

void draw_char(SDL_Renderer* r, char c, float x, float y, float w, float h, SDL_Color color) {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    
    if (c >= 'a' && c <= 'z') c -= 32;

    if (c >= '0' && c <= '9') {
        draw_digit(r, c - '0', x, y, w, h, color);
    } else if (c == '-') {
        SDL_RenderLine(r, x+2, y+h/2, x+w-2, y+h/2);
    } else if (c == '.') {
        SDL_RenderLine(r, x+w/2, y+h-2, x+w/2, y+h);
    } else if (c == ':') {
        SDL_RenderLine(r, x+w/2, y+h/3, x+w/2, y+h/3+1);
        SDL_RenderLine(r, x+w/2, y+2*h/3, x+w/2, y+2*h/3+1);
    } else if (c == '+') {
        SDL_RenderLine(r, x+w/2, y+h/4, x+w/2, y+3*h/4);
        SDL_RenderLine(r, x+w/4, y+h/2, x+3*w/4, y+h/2);
    } else if (c == '=') {
        SDL_RenderLine(r, x+2, y+h/3, x+w-2, y+h/3);
        SDL_RenderLine(r, x+2, y+2*h/3, x+w-2, y+2*h/3);
    } else if (c == 'J') {
        SDL_RenderLine(r, x+w-2, y, x+w-2, y+h-2);
        SDL_RenderLine(r, x+w-2, y+h-2, x+w/2, y+h);
        SDL_RenderLine(r, x+w/2, y+h, x+2, y+h-2);
        SDL_RenderLine(r, x+2, y+h-2, x+2, y+h/2);
    } else if (c == 'X') {
        SDL_RenderLine(r, x, y, x+w, y+h);
        SDL_RenderLine(r, x+w, y, x, y+h);
    } else if (c == 'Y') {
        SDL_RenderLine(r, x+w/2, y+h/2, x+w/2, y+h);
        SDL_RenderLine(r, x, y, x+w/2, y+h/2);
        SDL_RenderLine(r, x+w, y, x+w/2, y+h/2);
    } else if (c == 'A') {
        SDL_RenderLine(r, x+w/2, y, x, y+h);
        SDL_RenderLine(r, x+w/2, y, x+w, y+h);
        SDL_RenderLine(r, x+w/4, y+h/2+2, x+3*w/4, y+h/2+2);
    } else if (c == 'B') {
        SDL_RenderLine(r, x+2, y, x+2, y+h);
        SDL_RenderLine(r, x+2, y, x+w-2, y+h/4);
        SDL_RenderLine(r, x+w-2, y+h/4, x+2, y+h/2);
        SDL_RenderLine(r, x+2, y+h/2, x+w-2, y+3*h/4);
        SDL_RenderLine(r, x+w-2, y+3*h/4, x+2, y+h);
    } else if (c == 'C') {
        SDL_RenderLine(r, x+w-2, y, x+2, y);
        SDL_RenderLine(r, x+2, y, x+2, y+h);
        SDL_RenderLine(r, x+2, y+h, x+w-2, y+h);
    } else if (c == 'D') {
        SDL_RenderLine(r, x+2, y, x+2, y+h);
        SDL_RenderLine(r, x+2, y, x+w-4, y+h/4);
        SDL_RenderLine(r, x+w-4, y+h/4, x+w-4, y+3*h/4);
        SDL_RenderLine(r, x+w-4, y+3*h/4, x+2, y+h);
    } else if (c == 'E') {
        SDL_RenderLine(r, x+w-2, y, x+2, y);
        SDL_RenderLine(r, x+2, y, x+2, y+h);
        SDL_RenderLine(r, x+2, y+h, x+w-2, y+h);
        SDL_RenderLine(r, x+2, y+h/2, x+w-2, y+h/2);
    } else if (c == 'F') {
        SDL_RenderLine(r, x+w-2, y, x+2, y);
        SDL_RenderLine(r, x+2, y, x+2, y+h);
        SDL_RenderLine(r, x+2, y+h/2, x+w-2, y+h/2);
    } else if (c == 'G') {
        SDL_RenderLine(r, x+w-2, y, x+2, y);
        SDL_RenderLine(r, x+2, y, x+2, y+h);
        SDL_RenderLine(r, x+2, y+h, x+w-2, y+h);
        SDL_RenderLine(r, x+w-2, y+h, x+w-2, y+h/2);
        SDL_RenderLine(r, x+w-2, y+h/2, x+w/2, y+h/2);
    } else if (c == 'H') {
        SDL_RenderLine(r, x+2, y, x+2, y+h);
        SDL_RenderLine(r, x+w-2, y, x+w-2, y+h);
        SDL_RenderLine(r, x+2, y+h/2, x+w-2, y+h/2);
    } else if (c == 'I') {
        SDL_RenderLine(r, x+w/2, y, x+w/2, y+h);
        SDL_RenderLine(r, x, y, x+w, y);
        SDL_RenderLine(r, x, y+h, x+w, y+h);
    } else if (c == 'K') {
        SDL_RenderLine(r, x+2, y, x+2, y+h);
        SDL_RenderLine(r, x+2, y+h/2, x+w-2, y);
        SDL_RenderLine(r, x+2, y+h/2, x+w-2, y+h);
    } else if (c == 'L') {
        SDL_RenderLine(r, x+2, y, x+2, y+h);
        SDL_RenderLine(r, x+2, y+h, x+w-2, y+h);
    } else if (c == 'M') {
        SDL_RenderLine(r, x+2, y+h, x+2, y);
        SDL_RenderLine(r, x+2, y, x+w/2, y+h/2);
        SDL_RenderLine(r, x+w/2, y+h/2, x+w-2, y);
        SDL_RenderLine(r, x+w-2, y, x+w-2, y+h);
    } else if (c == 'N') {
        SDL_RenderLine(r, x+2, y+h, x+2, y);
        SDL_RenderLine(r, x+2, y, x+w-2, y+h);
        SDL_RenderLine(r, x+w-2, y+h, x+w-2, y);
    } else if (c == 'O') {
        SDL_RenderLine(r, x+2, y, x+w-2, y);
        SDL_RenderLine(r, x+2, y+h, x+w-2, y+h);
        SDL_RenderLine(r, x+2, y, x+2, y+h);
        SDL_RenderLine(r, x+w-2, y, x+w-2, y+h);
    } else if (c == 'P') {
        SDL_RenderLine(r, x+2, y, x+2, y+h);
        SDL_RenderLine(r, x+2, y, x+w-2, y+h/4);
        SDL_RenderLine(r, x+w-2, y+h/4, x+2, y+h/2);
    } else if (c == 'Q') {
        SDL_RenderLine(r, x+w/2, y, x+2, y+h/4);
        SDL_RenderLine(r, x+2, y+h/4, x+2, y+3*h/4);
        SDL_RenderLine(r, x+2, y+3*h/4, x+w/2, y+h);
        SDL_RenderLine(r, x+w/2, y+h, x+w-2, y+3*h/4);
        SDL_RenderLine(r, x+w-2, y+3*h/4, x+w-2, y+h/4);
        SDL_RenderLine(r, x+w-2, y+h/4, x+w/2, y);
        SDL_RenderLine(r, x+w/2, y+h/2, x+w, y+h);
    } else if (c == 'R') {
        SDL_RenderLine(r, x+2, y, x+2, y+h);
        SDL_RenderLine(r, x+2, y, x+w-2, y+h/4);
        SDL_RenderLine(r, x+w-2, y+h/4, x+2, y+h/2);
        SDL_RenderLine(r, x+2, y+h/2, x+w-2, y+h);
    } else if (c == 'S') {
        SDL_RenderLine(r, x+w-2, y, x+2, y);
        SDL_RenderLine(r, x+2, y, x+2, y+h/2);
        SDL_RenderLine(r, x+2, y+h/2, x+w-2, y+h/2);
        SDL_RenderLine(r, x+w-2, y+h/2, x+w-2, y+h);
        SDL_RenderLine(r, x+w-2, y+h, x+2, y+h);
    } else if (c == 'T') {
        SDL_RenderLine(r, x+w/2, y, x+w/2, y+h);
        SDL_RenderLine(r, x, y, x+w, y);
    } else if (c == 'U') {
        SDL_RenderLine(r, x+2, y, x+2, y+3*h/4);
        SDL_RenderLine(r, x+2, y+3*h/4, x+w/2, y+h);
        SDL_RenderLine(r, x+w/2, y+h, x+w-2, y+3*h/4);
        SDL_RenderLine(r, x+w-2, y+3*h/4, x+w-2, y);
    } else if (c == 'V') {
        SDL_RenderLine(r, x+2, y, x+w/2, y+h);
        SDL_RenderLine(r, x+w/2, y+h, x+w-2, y);
    } else if (c == 'W') {
        SDL_RenderLine(r, x+2, y, x+w/4, y+h);
        SDL_RenderLine(r, x+w/4, y+h, x+w/2, y+h/2);
        SDL_RenderLine(r, x+w/2, y+h/2, x+3*w/4, y+h);
        SDL_RenderLine(r, x+3*w/4, y+h, x+w-2, y);
    } else if (c == 'Z') {
        SDL_RenderLine(r, x, y, x+w, y);
        SDL_RenderLine(r, x+w, y, x, y+h);
        SDL_RenderLine(r, x, y+h, x+w, y+h);
    } else if (c == '(') {
        SDL_RenderLine(r, x+w-2, y, x+2, y+h/3);
        SDL_RenderLine(r, x+2, y+h/3, x+2, y+2*h/3);
        SDL_RenderLine(r, x+2, y+2*h/3, x+w-2, y+h);
    } else if (c == ')') {
        SDL_RenderLine(r, x+2, y, x+w-2, y+h/3);
        SDL_RenderLine(r, x+w-2, y+h/3, x+w-2, y+2*h/3);
        SDL_RenderLine(r, x+w-2, y+2*h/3, x+2, y+h);
    } else if (c == ',') {
        SDL_RenderLine(r, x+w/2, y+h-4, x+w/2-2, y+h);
    }
}

void draw_string(SDL_Renderer* r, const char* str, float x, float y, float char_w, float char_h, SDL_Color color) {
    for (int i = 0; str[i] != '\0'; i++) {
        draw_char(r, str[i], x + i * (char_w + 2), y, char_w, char_h, color);
    }
}

void draw_number(SDL_Renderer* r, float number, float x, float y, float char_w, float char_h, SDL_Color color, int decimals) {
    char buf[32];
    if (decimals == 0) {
        snprintf(buf, 32, "%d", (int)number);
    } else {
        snprintf(buf, 32, "%.*f", decimals, number);
    }
    draw_string(r, buf, x, y, char_w, char_h, color);
}

void draw_joint_label(SDL_Renderer* r, int joint_idx, float x, float y) {
    SDL_Color label_color = {255, 255, 100, 255};
    char label[16];
    snprintf(label, 16, "J%d", joint_idx);
    draw_string(r, label, x - 10, y - 20, 8, 12, label_color);
}

void draw_formula_box(SDL_Renderer* r) {
    if (!show_formula) return;

    int fx = 20, fy = 20, fw = 380, fh = current_mode == MODE_IK ? 200 : 160;

    SDL_SetRenderDrawColor(r, 10, 15, 25, 230);
    SDL_FRect bg = {fx, fy, fw, fh};
    SDL_RenderFillRect(r, &bg);
    SDL_SetRenderDrawColor(r, 0, 200, 255, 150);
    SDL_RenderRect(r, &bg);

    SDL_Color title_color = {0, 255, 200, 255};
    SDL_Color text_color = {200, 220, 240, 255};
    SDL_Color math_color = {255, 200, 100, 255};

    if (current_mode == MODE_IK) {
        draw_string(r, "INVERSE KINEMATICS (FABRIK)", fx + 10, fy + 10, 9, 14, title_color);
        draw_string(r, "Goal: Find joint angles given", fx + 10, fy + 35, 7, 11, text_color);
        draw_string(r, "target position (Tx, Ty)", fx + 10, fy + 50, 7, 11, text_color);

        draw_string(r, "Step 1: Forward Reach", fx + 10, fy + 75, 7, 11, math_color);
        draw_string(r, "  Set end-effector to target", fx + 10, fy + 90, 7, 11, text_color);
        draw_string(r, "  Pull each joint toward prev", fx + 10, fy + 105, 7, 11, text_color);

        draw_string(r, "Step 2: Backward Reach", fx + 10, fy + 125, 7, 11, math_color);
        draw_string(r, "  Fix base joint to origin", fx + 10, fy + 140, 7, 11, text_color);
        draw_string(r, "  Push each joint to length L", fx + 10, fy + 155, 7, 11, text_color);

        draw_string(r, "Repeat until |EE - Target| < ε", fx + 10, fy + 175, 7, 11, text_color);
    } else {
        draw_string(r, "FORWARD KINEMATICS", fx + 10, fy + 10, 9, 14, title_color);
        draw_string(r, "Given: Joint angles θi", fx + 10, fy + 35, 7, 11, text_color);
        draw_string(r, "Find: End-effector position", fx + 10, fy + 50, 7, 11, text_color);

        draw_string(r, "Xi = Xi-1 + Li * cos(θi)", fx + 10, fy + 80, 8, 12, math_color);
        draw_string(r, "Yi = Yi-1 + Li * sin(θi)", fx + 10, fy + 100, 8, 12, math_color);

        draw_string(r, "Where θi is absolute angle", fx + 10, fy + 125, 7, 11, text_color);
        draw_string(r, "from positive X-axis", fx + 10, fy + 140, 7, 11, text_color);
    }
}

void draw_button(SDL_Renderer* renderer, Button* b) {
    SDL_Color c = b->color;
    if (b->hovered) { c.r += 20; c.g += 20; c.b += 20; }
    if (b->clicked) { c.r -= 20; c.g -= 20; c.b -= 20; }

    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, &b->rect);

    if (b->hovered) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 200, 100);
        SDL_FRect border = {b->rect.x - 2, b->rect.y - 2, b->rect.w + 4, b->rect.h + 4};
        SDL_RenderRect(renderer, &border);
    }
    SDL_SetRenderDrawColor(renderer, 80, 90, 110, 255);
    SDL_RenderRect(renderer, &b->rect);

    SDL_Color text_color = {255, 255, 255, 255};
    draw_string(renderer, b->label, b->rect.x + 10, b->rect.y + b->rect.h/2 - 6, 8, 12, text_color);
}

void draw_input(SDL_Renderer* r, InputBox* inp) {
    SDL_Color bg = inp->active ? (SDL_Color){40, 50, 70, 255} : (SDL_Color){30, 35, 45, 255};
    SDL_SetRenderDrawColor(r, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderFillRect(r, &inp->rect);

    SDL_Color border = inp->active ? (SDL_Color){0, 255, 200, 255} : (inp->hovered ? (SDL_Color){100, 120, 140, 255} : (SDL_Color){60, 70, 90, 255});
    SDL_SetRenderDrawColor(r, border.r, border.g, border.b, border.a);
    SDL_RenderRect(r, &inp->rect);

    SDL_Color label_color = {150, 170, 190, 255};
    draw_string(r, inp->label, inp->rect.x, inp->rect.y - 16, 7, 10, label_color);

    SDL_Color text_color = {255, 255, 255, 255};
    draw_string(r, inp->text, inp->rect.x + 8, inp->rect.y + 8, 8, 14, text_color);

    if (inp->unit) {
        SDL_Color unit_color = {120, 140, 160, 255};
        draw_string(r, inp->unit, inp->rect.x + inp->rect.w + 5, inp->rect.y + 8, 7, 12, unit_color);
    }

    if (inp->active) {
        int text_w = inp->text_len * 10;
        SDL_SetRenderDrawColor(r, 0, 255, 200, 255);
        SDL_RenderLine(r, inp->rect.x + 8 + text_w, inp->rect.y + 5, inp->rect.x + 8 + text_w, inp->rect.y + inp->rect.h - 5);
    }
}
