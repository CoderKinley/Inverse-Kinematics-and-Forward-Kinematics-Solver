#include "kinematics.h"
#include "ui.h" // For updating UI inputs when IK solves
#include <math.h>
#include <string.h>
#include <stdio.h>

int num_segments = 3;
float segment_lengths[MAX_SEGMENTS] = { 180.0f, 150.0f, 120.0f, 100.0f, 80.0f, 60.0f };
Vec2 joints[MAX_SEGMENTS + 1];
float joint_angles[MAX_SEGMENTS];
float absolute_angles[MAX_SEGMENTS];
Vec2 base_pos;
Vec2 target_pos;
SimMode current_mode = MODE_IK;

void init_arm() {
    joints[0] = base_pos;
    for (int i = 1; i <= num_segments; i++) {
        joints[i] = add(joints[i-1], (Vec2){0, -segment_lengths[i-1]});
    }
}

void calculate_angles() {
    for (int i = 0; i < num_segments; i++) {
        Vec2 segment = sub(joints[i+1], joints[i]);
        absolute_angles[i] = atan2f(-segment.y, segment.x) * 180.0f / PI;
        if (absolute_angles[i] < 0) absolute_angles[i] += 360.0f;

        if (i == 0) {
            joint_angles[0] = absolute_angles[0];
        } else {
            Vec2 prev_segment = sub(joints[i], joints[i-1]);
            float prev_absolute = atan2f(-prev_segment.y, prev_segment.x) * 180.0f / PI;
            if (prev_absolute < 0) prev_absolute += 360.0f;

            float diff = absolute_angles[i] - prev_absolute;
            if (diff < -180.0f) diff += 360.0f;
            if (diff > 180.0f) diff -= 360.0f;
            joint_angles[i] = 180.0f + diff;
        }
    }
}

void solve_fk() {
    joints[0] = base_pos;
    float current_angle = -90.0f;

    for (int i = 0; i < num_segments; i++) {
        float angle_rad = deg2rad(current_angle + joint_angles[i]);
        Vec2 offset = {cosf(angle_rad) * segment_lengths[i], sinf(angle_rad) * segment_lengths[i]};
        joints[i+1] = add(joints[i], offset);
        current_angle += joint_angles[i] - 180.0f;
    }
    calculate_angles();
}

void solve_ik(Vec2 target) {
    float total_length = 0;
    for (int i = 0; i < num_segments; i++) total_length += segment_lengths[i];

    if (distance(base_pos, target) >= total_length - 0.1f) {
        Vec2 dir = normalize(sub(target, base_pos));
        for (int i = 1; i <= num_segments; i++) {
            joints[i] = add(joints[i-1], mul(dir, segment_lengths[i-1]));
        }
    } else {
        for (int iter = 0; iter < 20; iter++) {
            joints[num_segments] = target;
            for (int i = num_segments - 1; i >= 0; i--) {
                Vec2 dir = normalize(sub(joints[i], joints[i+1]));
                joints[i] = add(joints[i+1], mul(dir, segment_lengths[i]));
            }
            joints[0] = base_pos;
            for (int i = 1; i <= num_segments; i++) {
                Vec2 dir = normalize(sub(joints[i], joints[i-1]));
                joints[i] = add(joints[i-1], mul(dir, segment_lengths[i-1]));
            }
            if (distance(joints[num_segments], target) < 0.5f) break;
        }
    }
    calculate_angles();

    sync_inputs_with_angles();
}
