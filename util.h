#ifndef UTIL_H_
#define UTIL_H_
#include <stdlib.h>
#include <math.h>
#include <raylib.h>

#define new(ptr) malloc(sizeof(*ptr))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define TILE_SIZE 32
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define VECTOR2_ZERO (Vector2){0, 0}

Vector2 Vector2_normalized_multi(Vector2 v, float multi);
float get_distance(float x1, float y1, float x2, float y2);
void draw_text_with_bg(Font f, char* text, Vector2 dest, float size, float spacing, Color fg, Color bg);

#endif // UTIL_H_
