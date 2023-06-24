#ifndef COMMON_ENTITIES_H_
#define COMMON_ENTITIES_H_
#include "ecs.h"

struct Entity* rpg_player_create(float x, float y);
struct Entity* rpg_troll_create(float x, float y);
struct Entity* rpg_portal_create(float x, float y);
struct Entity* rpg_missile_create(struct Entity* const player, Camera2D* cam);
struct Entity* rpg_coin_create(float x, float y, int amount_of_gold);
struct Entity* rpg_label_create(float x, float y, const char* text, const Color col);
struct Entity* rpg_hurtbox_create(float x, float y, int dmg, unsigned int layer);
struct Entity* rpg_create_trail_ghost_from_entity(struct Entity* const e);
struct Entity* rpg_start_game_button(float x, float y);

#endif // COMMON_ENTITIES_H_
