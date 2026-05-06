#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#define PI 3.1415926535f

typedef struct { float x, y; } Vec2;

Vec2 add(Vec2 a, Vec2 b);
Vec2 sub(Vec2 a, Vec2 b);
Vec2 mul(Vec2 a, float s);
float length(Vec2 a);
Vec2 normalize(Vec2 a);
float distance(Vec2 a, Vec2 b);
float deg2rad(float d);
float rad2deg(float r);

#endif // MATH_UTILS_H
