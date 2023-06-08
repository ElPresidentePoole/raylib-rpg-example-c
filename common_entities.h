#ifndef COMMON_ENTITIES_H_
#define COMMON_ENTITIES_H_
#include "ecs.h"

Entity* e_troll_create(Texture2D* const tileset, float x, float y);
Entity* e_portal_create(Texture2D* const tileset, float x, float y);
Entity* e_missile_create(Texture2D *const tileset, Entity* const player, Camera2D* cam);

#endif // COMMON_ENTITIES_H_
