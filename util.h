#ifndef UTIL_H_
#define UTIL_H_
#include <math.h>
#include <raylib.h>

#define new(ptr) malloc(sizeof(*ptr))

Vector2 Vector2_normalized_multi(Vector2 v, float multi);

#endif // UTIL_H_
