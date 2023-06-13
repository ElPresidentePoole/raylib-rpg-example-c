#include <raylib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "common_entities.h"
#include "ecs.h"
#include "util.h"

const unsigned int LAYER_ENEMY_HURTBOX = 16; // 0b10000
const unsigned int LAYER_PLAYER = 8; // 0b01000
const unsigned int LAYER_TROLL = 4; // 0b00100
const unsigned int LAYER_MISSILE = 2; // 0b00010
const unsigned int LAYER_COIN = 1; // 0b00001

struct Entity* e_player_create(float x, float y) {
    struct Entity* player = ecs_entity_create();
    player->trans_c = new(player->trans_c);
    player->trans_c->rect = (Rectangle){x, y, TILE_SIZE, TILE_SIZE};
    player->trans_c->angle = 0.0;
    player->trans_c->origin = (Vector2){.x = TILE_SIZE / 2.0, .y = TILE_SIZE / 2.0};
    player->vel_c = new(player->vel_c);
    player->vel_c->vel = (Vector2){0, 0};
    player->vel_c->da = 0;
    player->tex_c = new(player->tex_c);
    player->tex_c->rect = (Rectangle){.x = 6 * TILE_SIZE, .y = 79 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
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

    return player;
}

struct Entity* e_portal_create(float x, float y) {
    struct Entity* portal = ecs_entity_create();
    portal->trans_c = new(portal->trans_c);
    portal->trans_c->rect = (Rectangle){x, y, TILE_SIZE, TILE_SIZE};
    portal->trans_c->angle = 0.0;
    portal->trans_c->origin = (Vector2){.x = TILE_SIZE / 2.0, .y = TILE_SIZE / 2.0};
    portal->tex_c = new(portal->tex_c);
    portal->tex_c->rect = (Rectangle){.x = 55 * TILE_SIZE, .y = 12 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
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
    enemy->trans_c->rect = (Rectangle){x, y, TILE_SIZE, TILE_SIZE};
    enemy->trans_c->angle = 0.0;
    enemy->trans_c->origin = (Vector2){.x = TILE_SIZE / 2.0, .y = TILE_SIZE / 2.0};
    enemy->vel_c = new(enemy->vel_c);
    enemy->vel_c->vel = (Vector2){0, 0};
    enemy->vel_c->da = 0;
    enemy->hp_c = new(enemy->hp_c);
    enemy->hp_c->hp = 5;
    enemy->tex_c = new(enemy->tex_c);
    enemy->tex_c->rect = (Rectangle){.x = 2 * TILE_SIZE, .y = 77 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
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
    return enemy;
}

struct Entity* e_missile_create(struct Entity* const player, Camera2D* cam) {
    static const int MISSILE_SPEED = 400;
    struct Entity* missile = ecs_entity_create();
    Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), *cam);
    int dx = player->trans_c->rect.x - mouse_pos.x;
    int dy = player->trans_c->rect.y - mouse_pos.y;
    double angle_between_player_and_mouse = atan2(dy, dx);
    missile->vel_c = new(missile->vel_c);
    missile->vel_c->vel = (Vector2){-cos(angle_between_player_and_mouse), -sin(angle_between_player_and_mouse)};
    missile->vel_c->vel.x *= MISSILE_SPEED;
    missile->vel_c->vel.y *= MISSILE_SPEED;
    // missile->vel_c->da = 0;
    missile->vel_c->da = 270;
    missile->trans_c = new(missile->trans_c);
    missile->trans_c->rect = (Rectangle){player->trans_c->rect.x, player->trans_c->rect.y, TILE_SIZE, TILE_SIZE};
    missile->trans_c->angle = angle_between_player_and_mouse * 180 / 3.1416 - 135;
    missile->trans_c->origin = (Vector2){.x = TILE_SIZE / 2, .y = TILE_SIZE / 2};
    missile->tex_c = new(missile->tex_c);
    // missile->tex_c->rect = (Rectangle){.x = 15 * TILE_SIZE, .y = 31 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    missile->tex_c->rect = (Rectangle){.x = 63 * TILE_SIZE, .y = 46 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    missile->life_c = new(missile->life_c);
    missile->life_c->time_remaining = 2.f;
    missile->col_c = new(missile->col_c);
    missile->col_c->layer = LAYER_MISSILE;
    missile->col_c->mask = 0;
    missile->col_c->hitbox = (Rectangle){player->trans_c->rect.x - TILE_SIZE / 2, player->trans_c->rect.y - TILE_SIZE / 2, TILE_SIZE, TILE_SIZE};
    missile->col_c->break_on_impact = true;
    missile->col_c->dmg = rand() % 2 + 2;
    return missile;
}

struct Entity* e_coin_create(float x, float y, int amount_of_gold) {
    struct Entity* coin = ecs_entity_create();

    coin->trans_c = new(coin->trans_c);
    coin->trans_c->rect = (Rectangle){x, y, TILE_SIZE, TILE_SIZE};
    coin->trans_c->angle = 0.0;
    coin->trans_c->origin = (Vector2){.x = TILE_SIZE / 2.f, .y = TILE_SIZE / 2.f};
    coin->tex_c = new(coin->tex_c);
    coin->tex_c->rect = (Rectangle){.x = 41 * TILE_SIZE, .y = 40 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
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
    label->trans_c->origin = (Vector2){TILE_SIZE / 2, TILE_SIZE / 2};
    label->trans_c->rect = (Rectangle){x, y, TILE_SIZE, TILE_SIZE};
    label->life_c = new(label->life_c);
    label->life_c->time_remaining = 2.f;
    label->vel_c = new(label->vel_c);
    label->vel_c->vel = (Vector2){.x = 0, .y = -50};
    label->vel_c->da = 0;
    return label;
}

struct Entity* e_hurtbox_create(float x, float y, int dmg) {
    struct Entity *troll_whack = ecs_entity_create();
    troll_whack->trans_c = new (troll_whack->trans_c);
    troll_whack->trans_c->rect = (Rectangle){x, y, TILE_SIZE, TILE_SIZE};
    troll_whack->trans_c->angle = 0;
    troll_whack->trans_c->origin = (Vector2){.x = TILE_SIZE / 2, .y = TILE_SIZE / 2};
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