#ifndef COMMON_ENTITIES_H_
#define COMMON_ENTITIES_H_
#include "ecs.h"

// XXX: just an idea for later, we make common entities from here
Entity* e_missile_create(Texture2D *const tileset, Entity* const player, Camera2D* cam);

#endif // COMMON_ENTITIES_H_
