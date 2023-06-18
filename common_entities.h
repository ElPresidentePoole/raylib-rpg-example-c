#ifndef COMMON_ENTITIES_H_
#define COMMON_ENTITIES_H_
#include "ecs.h"

struct Entity* e_player_create(float x, float y);
struct Entity* e_troll_create(float x, float y);
struct Entity* e_portal_create(float x, float y);
struct Entity* e_missile_create(struct Entity* const player, Camera2D* cam);
struct Entity* e_coin_create(float x, float y, int amount_of_gold);
struct Entity* e_label_create(float x, float y, const char* text, const Color col);
struct Entity* e_hurtbox_create(float x, float y, int dmg);
struct Entity* e_create_trail_ghost_from_entity(struct Entity* const e);
struct Entity* e_start_game_button(float x, float y);

#endif // COMMON_ENTITIES_H_
