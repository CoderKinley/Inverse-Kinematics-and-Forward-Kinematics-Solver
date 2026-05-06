#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "math_utils.h"
#include "kinematics.h"
#include "ui.h"
#include "render_utils.h"
#include <math.h>

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Robotic Arm Kinematics Simulator", 
                                          window_width, window_height, 
                                          SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_StartTextInput(window);

    update_layout();
    init_inputs();
    update_buttons_layout();
    init_arm();
    target_pos = (Vec2){content_width / 2.0f, content_height / 3.0f};
    solve_ik(target_pos);

    bool running = true;
    SDL_Event event;
    float mouse_x = 0, mouse_y = 0;
    bool mouse_down = false;
    bool prev_mouse_down = false;

    while (running) {
        prev_mouse_down = mouse_down;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;

                case SDL_EVENT_WINDOW_RESIZED:
                    window_width = event.window.data1;
                    window_height = event.window.data2;
                    update_layout();
                    update_buttons_layout();
                    init_arm();
                    if (current_mode == MODE_IK) {
                        solve_ik(target_pos);
                    } else {
                        solve_fk();
                    }
                    break;

                case SDL_EVENT_MOUSE_MOTION:
                    mouse_x = event.motion.x;
                    mouse_y = event.motion.y;
                    break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) mouse_down = true;
                    break;

                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (event.button.button == SDL_BUTTON_LEFT) mouse_down = false;
                    break;

                case SDL_EVENT_TEXT_INPUT:
                    for (int i = 0; i < num_segments; i++) handle_text_input(&inputs[i], event.text.text);
                    handle_text_input(&target_x_input, event.text.text);
                    handle_text_input(&target_y_input, event.text.text);
                    break;

                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_BACKSPACE) {
                        for (int i = 0; i < num_segments; i++) handle_backspace(&inputs[i]);
                        handle_backspace(&target_x_input);
                        handle_backspace(&target_y_input);
                    } else if (event.key.key == SDLK_F1) {
                        show_formula = !show_formula;
                    } else if (event.key.key == SDLK_TAB) {
                        bool found = false;
                        for (int i = 0; i < num_segments; i++) {
                            if (inputs[i].active) {
                                inputs[i].active = false;
                                inputs[(i+1) % num_segments].active = true;
                                found = true;
                                break;
                            }
                        }
                        if (!found) inputs[0].active = true;
                    }
                    break;
            }
        }

        bool mouse_pressed = mouse_down && !prev_mouse_down;

        // Update buttons
        update_button(&btn_add, mouse_x, mouse_y, mouse_pressed);
        update_button(&btn_sub, mouse_x, mouse_y, mouse_pressed);
        update_button(&btn_mode, mouse_x, mouse_y, mouse_pressed);
        update_button(&btn_solve, mouse_x, mouse_y, mouse_pressed);

        // Handle button clicks
        if (btn_add.clicked && num_segments < MAX_SEGMENTS) {
            segment_lengths[num_segments] = 100.0f;
            num_segments++;
            init_arm();
            if (current_mode == MODE_IK) solve_ik(target_pos);
            else solve_fk();
        }
        if (btn_sub.clicked && num_segments > MIN_SEGMENTS) {
            num_segments--;
            init_arm();
            if (current_mode == MODE_IK) solve_ik(target_pos);
            else solve_fk();
        }
        if (btn_mode.clicked) {
            current_mode = (current_mode == MODE_IK) ? MODE_FK : MODE_IK;
            init_arm();
            if (current_mode == MODE_IK) {
                target_pos = (Vec2){content_width / 2.0f, content_height / 3.0f};
                solve_ik(target_pos);
            } else {
                for (int i = 0; i < num_segments; i++) {
                    joint_angles[i] = 180.0f;
                    snprintf(inputs[i].text, MAX_INPUT_LEN, "180.0");
                    inputs[i].text_len = strlen(inputs[i].text);
                }
                solve_fk();
            }
        }
        if (btn_solve.clicked) {
            if (current_mode == MODE_IK) {
                target_pos.x = get_input_value(&target_x_input);
                target_pos.y = get_input_value(&target_y_input);
                solve_ik(target_pos);
            } else {
                for (int i = 0; i < num_segments; i++) {
                    joint_angles[i] = get_input_value(&inputs[i]);
                }
                solve_fk();
            }
        }

        // Update inputs
        for (int i = 0; i < num_segments; i++) {
            update_input(&inputs[i], mouse_x, mouse_y, mouse_pressed);
        }
        update_input(&target_x_input, mouse_x, mouse_y, mouse_pressed);
        update_input(&target_y_input, mouse_x, mouse_y, mouse_pressed);

        // IK Mode: Mouse click controls target
        if (current_mode == MODE_IK && mouse_pressed && mouse_x < content_width) {
            target_pos = (Vec2){mouse_x, mouse_y};
            snprintf(target_x_input.text, MAX_INPUT_LEN, "%.0f", target_pos.x);
            target_x_input.text_len = strlen(target_x_input.text);
            snprintf(target_y_input.text, MAX_INPUT_LEN, "%.0f", target_pos.y);
            target_y_input.text_len = strlen(target_y_input.text);
            solve_ik(target_pos);
        }

        // --- RENDERING ---
        SDL_SetRenderDrawColor(renderer, 12, 16, 24, 255);
        SDL_RenderClear(renderer);

        // Draw Grid
        SDL_SetRenderDrawColor(renderer, 25, 32, 45, 255);
        for (int x = 0; x < content_width; x += 40) {
            SDL_RenderLine(renderer, x, 0, x, content_height);
        }
        for (int y = 0; y < content_height; y += 40) {
            SDL_RenderLine(renderer, 0, y, content_width, y);
        }

        // Draw Coordinate axes
        SDL_SetRenderDrawColor(renderer, 60, 80, 100, 100);
        SDL_RenderLine(renderer, 0, base_pos.y, content_width, base_pos.y);
        SDL_RenderLine(renderer, base_pos.x, 0, base_pos.x, content_height);

        // Draw Arm Links
        SDL_Color bone_col = {0, 180, 200, 255};
        SDL_Color bone_glow = {0, 150, 180, 50};
        for (int i = 0; i < num_segments; i++) {
            draw_thick_line(renderer, joints[i], joints[i+1], 25.0f, bone_glow);
            draw_thick_line(renderer, joints[i], joints[i+1], 12.0f, bone_col);
        }

        // Draw Joints with labels
        SDL_Color joint_out = {0, 255, 150, 255};
        SDL_Color joint_in = {20, 30, 40, 255};
        for (int i = 0; i <= num_segments; i++) {
            draw_circle(renderer, joints[i].x, joints[i].y, 12.0f, joint_out);
            draw_circle(renderer, joints[i].x, joints[i].y, 7.0f, joint_in);
            draw_joint_label(renderer, i, joints[i].x, joints[i].y);
        }

        // Draw Base Platform
        SDL_FRect base_rect = {base_pos.x - 50, base_pos.y, 100, 30};
        SDL_SetRenderDrawColor(renderer, 60, 70, 90, 255);
        SDL_RenderFillRect(renderer, &base_rect);
        SDL_SetRenderDrawColor(renderer, 0, 255, 200, 255);
        SDL_RenderLine(renderer, base_pos.x - 50, base_pos.y, base_pos.x + 50, base_pos.y);
        draw_string(renderer, "BASE", base_pos.x - 15, base_pos.y + 8, 7, 10, (SDL_Color){0, 255, 200, 200});

        // Draw Target Reticle (IK mode only)
        if (current_mode == MODE_IK) {
            draw_circle(renderer, target_pos.x, target_pos.y, 18.0f, (SDL_Color){255, 120, 50, 40});
            draw_circle(renderer, target_pos.x, target_pos.y, 8.0f, (SDL_Color){255, 120, 50, 255});
            SDL_SetRenderDrawColor(renderer, 255, 120, 50, 200);
            SDL_RenderLine(renderer, target_pos.x - 25, target_pos.y, target_pos.x + 25, target_pos.y);
            SDL_RenderLine(renderer, target_pos.x, target_pos.y - 25, target_pos.x, target_pos.y + 25);
            draw_string(renderer, "TARGET", target_pos.x - 20, target_pos.y - 30, 7, 10, (SDL_Color){255, 120, 50, 255});
        }

        // Draw Angle Arcs
        for (int i = 0; i < num_segments; i++) {
            if (i > 0) {
                float start_angle = absolute_angles[i-1] - 90;
                float end_angle = absolute_angles[i] - 90;
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 100);
                float r = 30 + i * 5;
                for (float a = fminf(start_angle, end_angle); a <= fmaxf(start_angle, end_angle); a += 2) {
                    float rad = deg2rad(a);
                    SDL_RenderPoint(renderer, joints[i].x + cosf(rad) * r, joints[i].y + sinf(rad) * r);
                }
            }
        }

        // Draw Formula Box
        draw_formula_box(renderer);

        // --- DRAW SIDE UI PANEL ---
        SDL_FRect ui_panel = {content_width, 0, sidebar_width, window_height};
        SDL_SetRenderDrawColor(renderer, 18, 24, 34, 250);
        SDL_RenderFillRect(renderer, &ui_panel);
        SDL_SetRenderDrawColor(renderer, 0, 255, 200, 80);
        SDL_RenderLine(renderer, content_width, 0, content_width, window_height);

        // Panel Title
        SDL_Color title_color = {0, 255, 200, 255};
        draw_string(renderer, "TELEMETRY", content_width + 20, 20, 10, 16, title_color);

        // Mode indicator
        SDL_Color mode_color = current_mode == MODE_IK ? (SDL_Color){255, 150, 50, 255} : (SDL_Color){50, 200, 255, 255};
        draw_string(renderer, current_mode == MODE_IK ? "MODE: IK" : "MODE: FK", content_width + 20, 45, 8, 12, mode_color);

        // Buttons
        draw_button(renderer, &btn_add);
        draw_button(renderer, &btn_sub);
        draw_button(renderer, &btn_mode);
        draw_button(renderer, &btn_solve);

        // Target inputs (IK mode)
        if (current_mode == MODE_IK) {
            SDL_Color label_col = {150, 170, 190, 255};
            draw_string(renderer, "TARGET POSITION:", content_width + 20, 215, 8, 12, label_col);
            draw_input(renderer, &target_x_input);
            draw_input(renderer, &target_y_input);
        }

        // Joint angle inputs (FK mode) or display (IK mode)
        SDL_Color section_color = {150, 170, 190, 255};
        draw_string(renderer, current_mode == MODE_IK ? "JOINT ANGLES:" : "SET ANGLES:", 
                    content_width + 20, current_mode == MODE_IK ? 300.0f : 215.0f, 8, 12, section_color);

        for (int i = 0; i < num_segments; i++) {
            char joint_label[32];
            snprintf(joint_label, 32, "J%d", i);

            if (current_mode == MODE_FK) {
                draw_input(renderer, &inputs[i]);
            } else {
                // Display angles in IK mode
                SDL_Color val_color = {0, 255, 180, 255};
                draw_string(renderer, joint_label, content_width + 20, 330.0f + i * 30, 8, 12, section_color);
                draw_number(renderer, joint_angles[i], content_width + 70, 330.0f + i * 30, 8, 12, val_color, 1);
                draw_string(renderer, "deg", content_width + 140, 330.0f + i * 30, 7, 10, section_color);

                // Also show absolute angle
                draw_number(renderer, absolute_angles[i], content_width + 180, 330.0f + i * 30, 7, 10, (SDL_Color){200, 200, 100, 255}, 1);
                draw_string(renderer, "abs", content_width + 240, 330.0f + i * 30, 6, 9, (SDL_Color){150, 150, 100, 255});
            }
        }

        // End-effector info
        float info_y = window_height - 120;
        SDL_Color info_color = {200, 220, 240, 255};
        SDL_Color ee_color = {255, 165, 0, 255};

        draw_string(renderer, "END-EFFECTOR:", content_width + 20, info_y, 8, 12, info_color);
        draw_string(renderer, "X:", content_width + 20, info_y + 20, 7, 10, info_color);
        draw_number(renderer, joints[num_segments].x, content_width + 45, info_y + 20, 8, 12, ee_color, 1);
        draw_string(renderer, "Y:", content_width + 20, info_y + 40, 7, 10, info_color);
        draw_number(renderer, joints[num_segments].y, content_width + 45, info_y + 40, 8, 12, ee_color, 1);

        // Distance to target
        if (current_mode == MODE_IK) {
            float dist = distance(joints[num_segments], target_pos);
            draw_string(renderer, "ERROR:", content_width + 20, info_y + 65, 7, 10, (SDL_Color){255, 100, 100, 255});
            draw_number(renderer, dist, content_width + 75, info_y + 65, 8, 12, (SDL_Color){255, 100, 100, 255}, 2);
            draw_string(renderer, "px", content_width + 130, info_y + 65, 6, 9, (SDL_Color){255, 100, 100, 255});
        }

        // Help text
        draw_string(renderer, "F1: Toggle Formula", content_width + 20, window_height - 30, 6, 9, (SDL_Color){100, 120, 140, 255});
        draw_string(renderer, "TAB: Next Input", content_width + 20, window_height - 18, 6, 9, (SDL_Color){100, 120, 140, 255});

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_StopTextInput(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}