#include <raylib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "common_entities.h"
#include "ecs.h"
#include "util.h"
#include "scenes.h"
#include "magic.h"

struct Entity* rpg_player_create(float x, float y) {
    struct Entity* player = ecs_entity_create();
    player->trans_c = new(player->trans_c);
    player->trans_c->position = (Vector2){x, y};
    player->trans_c->uses_world_position = true;
    player->trans_c->angle = 0.0;
    player->trans_c->origin = (Vector2){.x = TILE_SIZE / 2.0, .y = TILE_SIZE / 2.0};
    player->trans_c->velocity = (Vector2){0, 0};
    player->trans_c->angular_velocity = 0.0;
    player->tex_c = new(player->tex_c);
    player->tex_c->source = (Rectangle){.x = 6 * TILE_SIZE, .y = 79 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    // player->alp_c = new(player->alp_c);
    // player->alp_c->alpha = 255U;
    // player->alp_c->fading = false;
    // player->alp_c->fade_per_second = 0;
    player->col_c = new(player->col_c);
    player->col_c->layer = LAYER_PLAYER;
    player->col_c->mask = LAYER_COIN | LAYER_ENEMY_HURTBOX;
    player->col_c->hitbox = (Rectangle){x-TILE_SIZE/2, y-TILE_SIZE/2, TILE_SIZE, TILE_SIZE};
    player->col_c->break_on_impact = false;
    player->col_c->dmg = 0;
    player->con_c = new(player->con_c);
    player->con_c->control = &e_control_player_controls;
    player->inv_c = new(player->inv_c);
    player->inv_c->gold = 0;
    player->hp_c = new(player->hp_c);
    player->hp_c->hp = 10;
    player->xpt_c = new(player->xpt_c);
    player->xpt_c->level = 1;
    player->xpt_c->xp_total = 0;
    player->xpt_c->xp_for_next_level = 5;
    player->mela_c = new(player->mela_c);
    player->mela_c->dmg = 10;
    player->rana_c = new(player->rana_c);
    player->rana_c->dmg = 2;
    player->rana_c->missile_func = &rpg_missile_create;

    return player;
}

struct Entity* rpg_portal_create(float x, float y) {
    struct Entity* portal = ecs_entity_create();
    portal->trans_c = new(portal->trans_c);
    portal->trans_c->position = (Vector2){x, y};
    portal->trans_c->angle = 0.0;
    portal->trans_c->origin = (Vector2){.x = TILE_SIZE / 2.0, .y = TILE_SIZE / 2.0};
    portal->trans_c->velocity = VECTOR2_ZERO;
    portal->trans_c->angular_velocity = 0.f; 
    portal->trans_c->uses_world_position = true;
    portal->tex_c = new(portal->tex_c);
    portal->tex_c->source = (Rectangle){.x = 55 * TILE_SIZE, .y = 12 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    portal->alp_c = new(portal->alp_c);
    portal->alp_c->alpha = 255U;
    portal->alp_c->fading = false;
    portal->alp_c->fade_per_second = 0;
    portal->tim_c = new(portal->tim_c);
    portal->tim_c->active = true;
    portal->tim_c->interval = 1.f;
    portal->tim_c->time_remaining = 1.f;
    portal->tim_c->repetitions = 2;
    portal->tim_c->on_timeout = &on_timeout_spawn_troll;
    return portal;
}

struct Entity* rpg_troll_create(float x, float y) {
    struct Entity* enemy = ecs_entity_create();
    enemy->trans_c = new(enemy->trans_c);
    enemy->trans_c->position = (Vector2){x, y};
    enemy->trans_c->angle = 0.0;
    enemy->trans_c->origin = (Vector2){.x = TILE_SIZE / 2.0, .y = TILE_SIZE / 2.0};
    enemy->trans_c->velocity = (Vector2){0, 0};
    enemy->trans_c->angular_velocity = 0;
    enemy->trans_c->uses_world_position = true;
    enemy->hp_c = new(enemy->hp_c);
    enemy->hp_c->hp = 5;
    enemy->tex_c = new(enemy->tex_c);
    enemy->tex_c->source = (Rectangle){.x = 2 * TILE_SIZE, .y = 77 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    enemy->alp_c = new(enemy->alp_c);
    enemy->alp_c->alpha = 255U;
    enemy->alp_c->fading = false;
    enemy->alp_c->fade_per_second = 0;
    enemy->col_c = new(enemy->col_c);
    enemy->col_c->layer = LAYER_TROLL;
    enemy->col_c->mask = LAYER_MISSILE;
    enemy->col_c->hitbox = (Rectangle){x - TILE_SIZE / 2, y - TILE_SIZE / 2, TILE_SIZE, TILE_SIZE};
    enemy->col_c->break_on_impact = false;
    enemy->col_c->dmg = 0;
    enemy->con_c = new(enemy->con_c);
    enemy->con_c->control = &e_control_run_towards_player;
    enemy->inv_c = new(enemy->inv_c);
    enemy->inv_c->gold = 1;
    enemy->xpr_c = new(enemy->xpr_c);
    enemy->xpr_c->xp_reward = 1;
    return enemy;
}

struct Entity* rpg_missile_create(float x, float y, double angle, int dmg) {
    static const int MISSILE_SPEED = 400;
    struct Entity* missile = ecs_entity_create();
    missile->trans_c = new(missile->trans_c);
    missile->trans_c->velocity = (Vector2){-cos(angle), -sin(angle)};
    missile->trans_c->velocity.x *= MISSILE_SPEED;
    missile->trans_c->velocity.y *= MISSILE_SPEED;
    missile->trans_c->angular_velocity = 270;
    missile->trans_c->position = (Vector2){x, y};
    missile->trans_c->uses_world_position = true;
    missile->trans_c->angle = angle * 180 / 3.1416 - 135;
    missile->trans_c->origin = (Vector2){.x = (float)TILE_SIZE / 2, .y = (float)TILE_SIZE / 2};
    missile->alp_c = new(missile->alp_c);
    // missile->alp_c->rect = (Rectangle){.x = 15 * TILE_SIZE, .y = 31 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    missile->alp_c->alpha = 255U;
    missile->alp_c->fading = false;
    missile->alp_c->fade_per_second = 0;
    missile->tex_c = new(missile->tex_c);
    missile->tex_c->source = (Rectangle){.x = 63 * TILE_SIZE, .y = 46 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    missile->life_c = new(missile->life_c);
    missile->life_c->time_remaining = 2.f;
    missile->col_c = new(missile->col_c);
    missile->col_c->layer = LAYER_MISSILE;
    missile->col_c->mask = 0;
    missile->col_c->hitbox = (Rectangle){x - (float)TILE_SIZE / 2, y - (float)TILE_SIZE / 2, TILE_SIZE, TILE_SIZE};
    missile->col_c->break_on_impact = true;
    missile->col_c->dmg = dmg;
    missile->tra_c = new(missile->tra_c);
    missile->tra_c->time_between_copies = 0.05f;
    missile->tra_c->time_remaining = 0.05f;
    missile->tra_c->remaining_copies = 15;

    return missile;
}

struct Entity* rpg_coin_create(float x, float y, int amount_of_gold) {
    struct Entity* coin = ecs_entity_create();

    coin->trans_c = new(coin->trans_c);
    coin->trans_c->position = (Vector2){x, y};
    coin->trans_c->uses_world_position = true;
    coin->trans_c->angle = 0.0;
    coin->trans_c->origin = (Vector2){.x = TILE_SIZE / 2.f, .y = TILE_SIZE / 2.f};
    coin->trans_c->velocity = VECTOR2_ZERO;
    coin->trans_c->angular_velocity = 0.f; 
    coin->tex_c = new(coin->tex_c);
    coin->tex_c->source = (Rectangle){.x = 41 * TILE_SIZE, .y = 40 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    coin->alp_c = new(coin->alp_c);
    coin->alp_c->alpha = 255U;
    coin->alp_c->fading = false;
    coin->alp_c->fade_per_second = 0;
    coin->col_c = new(coin->col_c);
    coin->col_c->layer = LAYER_COIN;
    coin->col_c->mask = 0;
    coin->col_c->hitbox = (Rectangle){x - TILE_SIZE / 2.f, y - TILE_SIZE / 2.f, TILE_SIZE, TILE_SIZE};
    coin->col_c->break_on_impact = true;
    coin->col_c->dmg = 0;
    coin->pic_c = new(coin->pic_c);
    coin->pic_c->gold_reward = 1;

    return coin;
}

struct Entity* rpg_label_create(float x, float y, const char* text, const Color col) {
    assert(strlen(text) < 128);
    struct Entity* label = ecs_entity_create();
    label->lab_c = new(label->lab_c);
    sprintf(label->lab_c->text, "%s", text); // this is probably not good practice but oh well
    label->lab_c->color = col;
    label->lab_c->size = 12.f;
    label->trans_c = new(label->trans_c);
    label->trans_c->angle = 0.f;
    label->trans_c->origin = (Vector2){TILE_SIZE / 2.f, TILE_SIZE / 2.f};
    label->trans_c->position = (Vector2){x, y};
    label->trans_c->uses_world_position = true;
    label->trans_c->velocity = (Vector2){.x = 0, .y = -50};
    label->trans_c->angular_velocity = 0;
    label->life_c = new(label->life_c);
    label->life_c->time_remaining = 2.f;
    label->alp_c = new(label->alp_c);
    label->alp_c->alpha = 255U;
    label->alp_c->fading = true;
    label->alp_c->fade_per_second = 255U  / 2U;
    return label;
}

struct Entity* rpg_hurtbox_create(float x, float y, int dmg, unsigned int layer) {
    struct Entity *troll_whack = ecs_entity_create();
    troll_whack->trans_c = new (troll_whack->trans_c);
    troll_whack->trans_c->position = (Vector2){x, y};
    troll_whack->trans_c->uses_world_position = true;
    troll_whack->trans_c->angle = 0;
    troll_whack->trans_c->origin = (Vector2){.x = TILE_SIZE / 2, .y = TILE_SIZE / 2};
    troll_whack->trans_c->velocity = VECTOR2_ZERO;
    troll_whack->trans_c->angular_velocity = 0.f; 
    troll_whack->life_c = new (troll_whack->life_c);
    troll_whack->life_c->time_remaining = 0.1f;
    troll_whack->col_c = new (troll_whack->col_c);
    troll_whack->col_c->layer = layer;
    troll_whack->col_c->mask = 0;
    troll_whack->col_c->hitbox = (Rectangle){x - TILE_SIZE / 2, y - TILE_SIZE / 2, TILE_SIZE, TILE_SIZE};
    troll_whack->col_c->break_on_impact = true;
    troll_whack->col_c->dmg = dmg;
    return troll_whack;
}

struct Entity* rpg_create_trail_ghost_from_entity(struct Entity* const e) {
    struct Entity* ghost = ecs_entity_create();
    ghost->trans_c = new (ghost->trans_c);
    ghost->trans_c->position = (Vector2){e->trans_c->position.x, e->trans_c->position.y};
    ghost->trans_c->uses_world_position = true;
    ghost->trans_c->angle = e->trans_c->angle;
    ghost->trans_c->origin = (Vector2){e->trans_c->origin.x, e->trans_c->origin.y};
    ghost->trans_c->velocity = VECTOR2_ZERO;
    ghost->trans_c->angular_velocity = 0.f; 
    ghost->life_c = new (ghost->life_c);
    ghost->life_c->time_remaining = 0.5f; // would TrailComponents cause a muckup if the Entity is freed?
    ghost->tex_c = new(ghost->tex_c);
    ghost->tex_c->source = (Rectangle){e->tex_c->source.x, e->tex_c->source.y, TILE_SIZE, TILE_SIZE};
    ghost->alp_c = new(ghost->alp_c);
    ghost->alp_c->alpha = 255U;
    ghost->alp_c->fading = true;
    ghost->alp_c->fade_per_second = 255 / ghost->life_c->time_remaining; // this shouldn't be a float though
    return ghost;
}

struct Entity* rpg_start_game_button(float x, float y) {
  struct Entity* button = ecs_entity_create();
  button->lab_c = new(button->lab_c);
  button->lab_c->color = GOLD;
  button->lab_c->size = 24.f;
  sprintf(button->lab_c->text, "%s", "Start Game"); // this is probably not good practice but oh well
  button->alp_c = new(button->alp_c);
  button->alp_c->alpha = 255U;
  button->alp_c->fading = false;
  button->alp_c->fade_per_second = 0;
  button->trans_c = new(button->trans_c);
  button->trans_c->angle = 0.f;
  button->trans_c->angular_velocity = 0.f;
  button->trans_c->velocity = VECTOR2_ZERO;
  button->trans_c->origin = (Vector2){ 50, 25 };
  button->trans_c->position = (Vector2){ x, y };
  button->trans_c->uses_world_position = false;
  button->cli_c = new(button->cli_c);
  button->cli_c->on_click = &scene_in_game_setup;
  button->cli_c->size = (struct Dimensions){200.f, 100.f};
  return button;
}

struct Entity* rpg_spawn_wave_manager() {
  struct Entity* manager = ecs_entity_create();
  manager->tim_c = new(manager->tim_c);
  manager->tim_c->active = true;
  manager->tim_c->interval = 1.f;
  manager->tim_c->time_remaining = 1.f;
  manager->tim_c->repetitions = -1;
  manager->tim_c->on_timeout = &on_timeout_manage_wave;
  manager->trans_c = new(manager->trans_c);
  manager->trans_c->angle = 0.f;
  manager->trans_c->angular_velocity = 0.f;
  manager->trans_c->velocity = VECTOR2_ZERO;
  manager->trans_c->origin = (Vector2){ 0, 0 };
  manager->trans_c->position = (Vector2){ 10, SCREEN_HEIGHT - 100 };
  manager->trans_c->uses_world_position = false;
  manager->lab_c = new(manager->lab_c);
  strcpy(manager->lab_c->text, "asdsadad");
  manager->lab_c->color = BLUE;
  manager->lab_c->size = 30.f;
  return manager;
}
