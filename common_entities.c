#include <raylib.h>
#include "common_entities.h"
#include "util.h"

Entity* e_portal_create(Texture2D* const tileset, float x, float y) {
    Entity* portal = ecs_entity_create();
    portal->trans_c = new(portal->trans_c);
    portal->trans_c->rect = (Rectangle){x, y, TILE_SIZE, TILE_SIZE};
    portal->trans_c->angle = 0.0;
    portal->trans_c->origin = (Vector2){.x = TILE_SIZE / 2.0, .y = TILE_SIZE / 2.0};
    portal->tex_c = new(portal->tex_c);
    portal->tex_c->tex = *tileset;
    portal->tex_c->rect = (Rectangle){.x = 55 * TILE_SIZE, .y = 12 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    return portal;
}

Entity* e_troll_create(Texture2D* const tileset, float x, float y) {
    Entity* enemy = ecs_entity_create();
    enemy->trans_c = new(enemy->trans_c);
    enemy->trans_c->rect = (Rectangle){x, y, TILE_SIZE, TILE_SIZE};
    enemy->trans_c->angle = 0.0;
    enemy->trans_c->origin = (Vector2){.x = TILE_SIZE / 2.0, .y = TILE_SIZE / 2.0};
    enemy->vel_c = new(enemy->vel_c);
    enemy->vel_c->vel = (Vector2){0, 0};
    enemy->tex_c = new(enemy->tex_c);
    enemy->tex_c->tex = *tileset;
    enemy->tex_c->rect = (Rectangle){.x = 2 * TILE_SIZE, .y = 77 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    enemy->col_c = new(enemy->col_c);
    enemy->col_c->layer = 0b1000;
    enemy->col_c->mask =  0b0100;
    enemy->col_c->hitbox = (Rectangle){x - TILE_SIZE / 2, y - TILE_SIZE / 2, TILE_SIZE, TILE_SIZE};
    return enemy;
}

Entity* e_missile_create(Texture2D *const tileset, Entity* const player, Camera2D* cam) {
    const int MISSILE_SPEED = 200;
    Entity* missile = ecs_entity_create();
    Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), *cam);
    int dx = player->trans_c->rect.x - mouse_pos.x;
    int dy = player->trans_c->rect.y - mouse_pos.y;
    double angle_between_player_and_mouse = atan2(dy, dx);
    missile->vel_c = new(missile->vel_c);
    missile->vel_c->vel = (Vector2){-cos(angle_between_player_and_mouse), -sin(angle_between_player_and_mouse)};
    missile->vel_c->vel.x *= MISSILE_SPEED;
    missile->vel_c->vel.y *= MISSILE_SPEED;
    missile->trans_c = new(missile->trans_c);
    missile->trans_c->rect = (Rectangle){player->trans_c->rect.x, player->trans_c->rect.y, TILE_SIZE, TILE_SIZE};
    missile->trans_c->angle = angle_between_player_and_mouse * 180 / 3.1416 + 90;
    missile->trans_c->origin = (Vector2){.x = TILE_SIZE / 2, .y = TILE_SIZE / 2};

    missile->tex_c = new(missile->tex_c);
    missile->tex_c->tex = *tileset;
    missile->tex_c->rect = (Rectangle){.x = 15 * TILE_SIZE, .y = 31 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    missile->life_c = new(missile->life_c);
    missile->life_c->time_remaining = 2.f;
    missile->col_c = new(missile->col_c);
    missile->col_c->layer = 0b0100;
    missile->col_c->mask =  0b0000;
    missile->col_c->hitbox = (Rectangle){player->trans_c->rect.x - TILE_SIZE / 2, player->trans_c->rect.y - TILE_SIZE / 2, TILE_SIZE, TILE_SIZE};
    return missile;
}
