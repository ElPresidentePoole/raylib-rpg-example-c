#include "util.h"

Vector2 Vector2_normalized_multi(Vector2 v, float multi) {
    if(v.x == 0 && v.y == 0) return v; // Vectors with a magnitude of 0 can not be normalized.
    float mag = sqrt(v.x * v.x + v.y * v.y);
    return (Vector2){.x = v.x / mag * multi, .y = v.y / mag * multi};
}

float get_distance(float x1, float y1, float x2, float y2) {
    return sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2));
}