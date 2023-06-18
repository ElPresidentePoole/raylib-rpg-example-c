#include <raylib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "common_entities.h"
#include "ecs.h"
#include "util.h"
#include "scenes.h"

const unsigned int LAYER_ENEMY_HURTBOX = 16; // 0b10000
const unsigned int LAYER_PLAYER = 8; // 0b01000
const unsigned int LAYER_TROLL = 4; // 0b00100
const unsigned int LAYER_MISSILE = 2; // 0b00010
const unsigned int LAYER_COIN = 1; // 0b00001

struct Entity* e_player_create(float x, float y) {
    struct Entity* player = ecs_entity_create();
    player->trans_c = new(player->trans_c);
    player->trans_c->position = (Vector2){x, y};
    player->trans_c->angle = 0.0;
    player->trans_c->origin = (Vector2){.x = TILE_SIZE / 2.0, .y = TILE_SIZE / 2.0};
    player->trans_c->velocity = (Vector2){0, 0};
    player->trans_c->angular_velocity = 0.0;
    player->tex_c = new(player->tex_c);
    player->tex_c->source = (Rectangle){.x = 6 * TILE_SIZE, .y = 79 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    player->vis_c = new(player->vis_c);
    player->vis_c->alpha = 255U;
    player->vis_c->fading = false;
    player->vis_c->fade_per_second = 0;
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

    return player;
}

struct Entity* e_portal_create(float x, float y) {
    struct Entity* portal = ecs_entity_create();
    portal->trans_c = new(portal->trans_c);
    portal->trans_c->position = (Vector2){x, y};
    portal->trans_c->angle = 0.0;
    portal->trans_c->origin = (Vector2){.x = TILE_SIZE / 2.0, .y = TILE_SIZE / 2.0};
    portal->trans_c->velocity = VECTOR2_ZERO;
    portal->trans_c->angular_velocity = 0.f; 
    portal->tex_c = new(portal->tex_c);
    portal->tex_c->source = (Rectangle){.x = 55 * TILE_SIZE, .y = 12 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    portal->vis_c = new(portal->vis_c);
    portal->vis_c->alpha = 255U;
    portal->vis_c->fading = false;
    portal->vis_c->fade_per_second = 0;
    portal->tim_c = new(portal->tim_c);
    portal->tim_c->active = true;
    portal->tim_c->interval = 1.f;
    portal->tim_c->time_remaining = 1.f;
    portal->tim_c->repeating = true;
    portal->tim_c->on_timeout = &on_timeout_spawn_troll;
    return portal;
}

struct Entity* e_troll_create(float x, float y) {
    struct Entity* enemy = ecs_entity_create();
    enemy->trans_c = new(enemy->trans_c);
    enemy->trans_c->position = (Vector2){x, y};
    enemy->trans_c->angle = 0.0;
    enemy->trans_c->origin = (Vector2){.x = TILE_SIZE / 2.0, .y = TILE_SIZE / 2.0};
    enemy->trans_c->velocity = (Vector2){0, 0};
    enemy->trans_c->angular_velocity = 0;
    enemy->hp_c = new(enemy->hp_c);
    enemy->hp_c->hp = 5;
    enemy->tex_c = new(enemy->tex_c);
    enemy->tex_c->source = (Rectangle){.x = 2 * TILE_SIZE, .y = 77 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    enemy->vis_c = new(enemy->vis_c);
    enemy->vis_c->alpha = 255U;
    enemy->vis_c->fading = false;
    enemy->vis_c->fade_per_second = 0;
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

struct Entity* e_missile_create(struct Entity* const player, Camera2D* cam) {
    static const int MISSILE_SPEED = 400;
    struct Entity* missile = ecs_entity_create();
    Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), *cam);
    int dx = player->trans_c->position.x - mouse_pos.x;
    int dy = player->trans_c->position.y - mouse_pos.y;
    double angle_between_player_and_mouse = atan2(dy, dx);
    missile->trans_c = new(missile->trans_c);
    missile->trans_c->velocity = (Vector2){-cos(angle_between_player_and_mouse), -sin(angle_between_player_and_mouse)};
    missile->trans_c->velocity.x *= MISSILE_SPEED;
    missile->trans_c->velocity.y *= MISSILE_SPEED;
    missile->trans_c->angular_velocity = 270;
    missile->trans_c->position = (Vector2){player->trans_c->position.x, player->trans_c->position.y};
    missile->trans_c->angle = angle_between_player_and_mouse * 180 / 3.1416 - 135;
    missile->trans_c->origin = (Vector2){.x = (float)TILE_SIZE / 2, .y = (float)TILE_SIZE / 2};
    missile->vis_c = new(missile->vis_c);
    // missile->vis_c->rect = (Rectangle){.x = 15 * TILE_SIZE, .y = 31 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    missile->vis_c->alpha = 255U;
    missile->vis_c->fading = false;
    missile->vis_c->fade_per_second = 0;
    missile->tex_c = new(missile->tex_c);
    missile->tex_c->source = (Rectangle){.x = 63 * TILE_SIZE, .y = 46 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    missile->life_c = new(missile->life_c);
    missile->life_c->time_remaining = 2.f;
    missile->col_c = new(missile->col_c);
    missile->col_c->layer = LAYER_MISSILE;
    missile->col_c->mask = 0;
    missile->col_c->hitbox = (Rectangle){player->trans_c->position.x - (float)TILE_SIZE / 2, player->trans_c->position.y - (float)TILE_SIZE / 2, TILE_SIZE, TILE_SIZE};
    missile->col_c->break_on_impact = true;
    missile->col_c->dmg = rand() % 5 + 3;
    missile->tra_c = new(missile->tra_c);
    missile->tra_c->time_between_copies = 0.05f;
    missile->tra_c->remaining_copies = 15;
    return missile;
}

struct Entity* e_coin_create(float x, float y, int amount_of_gold) {
    struct Entity* coin = ecs_entity_create();

    coin->trans_c = new(coin->trans_c);
    coin->trans_c->position = (Vector2){x, y};
    coin->trans_c->angle = 0.0;
    coin->trans_c->origin = (Vector2){.x = TILE_SIZE / 2.f, .y = TILE_SIZE / 2.f};
    coin->trans_c->velocity = VECTOR2_ZERO;
    coin->trans_c->angular_velocity = 0.f; 
    coin->tex_c = new(coin->tex_c);
    coin->tex_c->source = (Rectangle){.x = 41 * TILE_SIZE, .y = 40 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    coin->vis_c = new(coin->vis_c);
    coin->vis_c->alpha = 255U;
    coin->vis_c->fading = false;
    coin->vis_c->fade_per_second = 0;
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

struct Entity* e_label_create(float x, float y, const char* text, const Color col) {
    assert(strlen(text) < 128);
    struct Entity* label = ecs_entity_create();
    label->lab_c = new(label->lab_c);
    sprintf(label->lab_c->text, "%s", text); // this is probably not good practice but oh well
    label->lab_c->color = col;
    label->trans_c = new(label->trans_c);
    label->trans_c->angle = 0.f;
    label->trans_c->origin = (Vector2){TILE_SIZE / 2.f, TILE_SIZE / 2.f};
    label->trans_c->position = (Vector2){x, y};
    label->trans_c->velocity = (Vector2){.x = 0, .y = -50};
    label->trans_c->angular_velocity = 0;
    label->life_c = new(label->life_c);
    label->life_c->time_remaining = 2.f;
    label->vis_c = new(label->vis_c);
    label->vis_c->alpha = 255U;
    label->vis_c->fading = true;
    label->vis_c->fade_per_second = 255U  / 2U;
    return label;
}

struct Entity* e_hurtbox_create(float x, float y, int dmg) {
    struct Entity *troll_whack = ecs_entity_create();
    troll_whack->trans_c = new (troll_whack->trans_c);
    troll_whack->trans_c->position = (Vector2){x, y};
    troll_whack->trans_c->angle = 0;
    troll_whack->trans_c->origin = (Vector2){.x = TILE_SIZE / 2, .y = TILE_SIZE / 2};
    troll_whack->trans_c->velocity = VECTOR2_ZERO;
    troll_whack->trans_c->angular_velocity = 0.f; 
    troll_whack->life_c = new (troll_whack->life_c);
    troll_whack->life_c->time_remaining = 0.1f;
    troll_whack->col_c = new (troll_whack->col_c);
    troll_whack->col_c->layer = LAYER_ENEMY_HURTBOX;
    troll_whack->col_c->mask = 0;
    troll_whack->col_c->hitbox = (Rectangle){x - TILE_SIZE / 2, y - TILE_SIZE / 2, TILE_SIZE, TILE_SIZE};
    troll_whack->col_c->break_on_impact = true;
    troll_whack->col_c->dmg = dmg;
    return troll_whack;
}

struct Entity* e_create_trail_ghost_from_entity(struct Entity* const e) {
    struct Entity* ghost = ecs_entity_create();
    ghost->trans_c = new (ghost->trans_c);
    ghost->trans_c->position = (Vector2){e->trans_c->position.x, e->trans_c->position.y};
    ghost->trans_c->angle = e->trans_c->angle;
    ghost->trans_c->origin = (Vector2){e->trans_c->origin.x, e->trans_c->origin.y};
    ghost->trans_c->velocity = VECTOR2_ZERO;
    ghost->trans_c->angular_velocity = 0.f; 
    ghost->life_c = new (ghost->life_c);
    ghost->life_c->time_remaining = 0.5f; // would TrailComponents cause a muckup if the Entity is freed?
    ghost->tex_c = new(ghost->tex_c);
    ghost->tex_c->source = (Rectangle){e->tex_c->source.x, e->tex_c->source.y, TILE_SIZE, TILE_SIZE};
    ghost->vis_c = new(ghost->vis_c);
    ghost->vis_c->alpha = 255U;
    ghost->vis_c->fading = true;
    ghost->vis_c->fade_per_second = 255 / ghost->life_c->time_remaining; // this shouldn't be a float though
    return ghost;
}

struct Entity* e_start_game_button(float x, float y) {
  struct Entity* button = ecs_entity_create();
  button->lab_c = new(button->lab_c);
  button->lab_c->color = BLACK;
  sprintf(button->lab_c->text, "%s", "Start Game"); // this is probably not good practice but oh well
  button->vis_c = new(button->vis_c);
  button->vis_c->alpha = 255U;
  button->vis_c->fading = false;
  button->vis_c->fade_per_second = 0;
  button->cli_c = new(button->cli_c);
  button->cli_c->clickbox = (Rectangle){ x, y, 100.f, 50.f };  //  TODO: dimensions component width/height
  button->cli_c->on_click = &scene_in_game_setup;
  return button;
}
