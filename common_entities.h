#ifndef COMMON_ENTITIES_H_
#define COMMON_ENTITIES_H_
#include "ecs.h"

struct Entity* e_player_create(float x, float y);
struct Entity* e_troll_create(float x, float y);
struct Entity* e_portal_create(float x, float y);
struct Entity* e_missile_create(struct Entity* const player, Camera2D* cam);

#endif // COMMON_ENTITIES_H_
