#include "util.h"

Vector2 Vector2_normalized_multi(Vector2 v, float multi) {
    if(v.x == 0 && v.y == 0) return v; // Vectors with a magnitude of 0 can not be normalized.
    float mag = sqrt(v.x * v.x + v.y * v.y);
    return (Vector2){.x = v.x / mag * multi, .y = v.y / mag * multi};
}
