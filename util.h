#ifndef UTIL_H_
#define UTIL_H_
#include <stdlib.h>
#include <math.h>
#include <raylib.h>

#define new(ptr) malloc(sizeof(*ptr))
#define TILE_SIZE 32
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

Vector2 Vector2_normalized_multi(Vector2 v, float multi);

#endif // UTIL_H_
