#include "math_utils.h"
#include <math.h>

Vec2 add(Vec2 a, Vec2 b) { return (Vec2){a.x + b.x, a.y + b.y}; }
Vec2 sub(Vec2 a, Vec2 b) { return (Vec2){a.x - b.x, a.y - b.y}; }
Vec2 mul(Vec2 a, float s) { return (Vec2){a.x * s, a.y * s}; }
float length(Vec2 a) { return sqrtf(a.x * a.x + a.y * a.y); }
Vec2 normalize(Vec2 a) { 
    float l = length(a); 
    if (l == 0) return (Vec2){0, 0};
    return (Vec2){a.x / l, a.y / l}; 
}
float distance(Vec2 a, Vec2 b) { return length(sub(a, b)); }
float deg2rad(float d) { return d * PI / 180.0f; }
float rad2deg(float r) { return r * 180.0f / PI; }
