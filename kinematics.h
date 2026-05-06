#ifndef KINEMATICS_H
#define KINEMATICS_H

#include "math_utils.h"
#include <stdbool.h>

#define MAX_SEGMENTS 6
#define MIN_SEGMENTS 2

typedef enum { MODE_IK, MODE_FK } SimMode;

extern int num_segments;
extern float segment_lengths[MAX_SEGMENTS];
extern Vec2 joints[MAX_SEGMENTS + 1];
extern float joint_angles[MAX_SEGMENTS];
extern float absolute_angles[MAX_SEGMENTS];
extern Vec2 base_pos;
extern Vec2 target_pos;
extern SimMode current_mode;

void init_arm(void);
void calculate_angles(void);
void solve_fk(void);
void solve_ik(Vec2 target);

#endif // KINEMATICS_H
