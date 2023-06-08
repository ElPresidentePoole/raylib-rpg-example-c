#include "common_entities.h"
#include "util.h"

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
    missile->col_c->hitbox = (Rectangle){player->trans_c->rect.x, player->trans_c->rect.y, TILE_SIZE, TILE_SIZE};
    return missile;
}
