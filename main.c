#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ecs.h"
#include "util.h"

#define TILE_SIZE 32

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "rpg");
    // InitAudioDevice();

    Texture2D tileset = LoadTexture("./tileset.png");

    SetTargetFPS(60);

    Camera2D cam = { 0 };
    cam.offset = (Vector2){ screenWidth / 2.f - TILE_SIZE / 2.f, screenHeight / 2.f - TILE_SIZE / 2.f};
    cam.rotation = 0.f;
    cam.zoom = 2.f;

    Entity* player = ecs_entity_create();
    player->pos_c = new(player->pos_c);
    player->pos_c->pos = (Vector2){0, 0};
    player->vel_c = new(player->vel_c);
    player->vel_c->vel = (Vector2){0, 0};
    player->tex_c = new(player->tex_c);
    player->tex_c->tex = tileset;
    player->tex_c->rect = (Rectangle){.x = 6 * TILE_SIZE, .y = 79 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
    const int PLAYER_SPEED = 100; // should this be a component or something?

    EntityContainer* world = ecs_entitycontainer_create();
    // ecs_entitycontainer_push(world, missile);
    ecs_entitycontainer_push(world, player);

    ecs_entitycontainer_add_system(world, &ecs_system_render); // render has to go first to avoid lagging behind
    ecs_entitycontainer_add_system(world, &ecs_system_movement);
    ecs_entitycontainer_add_system(world, &ecs_system_despawn);

    while (!WindowShouldClose())
    {
        Vector2 player_movement = (Vector2){.x = 0, .y = 0};
        if(IsKeyDown(KEY_S)) {
            player_movement.y += 1;
        }
        if(IsKeyDown(KEY_W)) {
            player_movement.y -= 1;
        }
        if(IsKeyDown(KEY_D)) {
            player_movement.x += 1;
        }
        if(IsKeyDown(KEY_A)) {
            player_movement.x -= 1;
        }
        struct Vector2 pm_normal = Vector2_normalized_multi(player_movement, PLAYER_SPEED);
        player->vel_c->vel.x = pm_normal.x;
        player->vel_c->vel.y = pm_normal.y;
        printf("%f %f\n", player->vel_c->vel.x, player->vel_c->vel.y);
        cam.target = (Vector2){ player->pos_c->pos.x, player->pos_c->pos.y };
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            const int MISSILE_SPEED = 100;
            Entity* missile = ecs_entity_create();
            missile->pos_c = new(missile->pos_c);
            missile->pos_c->pos = (Vector2){player->pos_c->pos.x, player->pos_c->pos.y};
            Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), cam);
            int dx = player->pos_c->pos.x - mouse_pos.x;
            int dy = player->pos_c->pos.y - mouse_pos.y;
            double angle_between_player_and_mouse = atan2(dy, dx);
            missile->vel_c = new(missile->vel_c);
            missile->vel_c->vel = (Vector2){-cos(angle_between_player_and_mouse), -sin(angle_between_player_and_mouse)};
            missile->vel_c->vel.x *= MISSILE_SPEED;
            missile->vel_c->vel.y *= MISSILE_SPEED;

            //GetMousePosition()
            //missile->vel_c->vel = (Vector2){MISSILE_SPEED * cos(angle_to_mouse), MISSILE_SPEED * sin(angle_to_mouse)};
            missile->tex_c = new(missile->tex_c);
            missile->tex_c->tex = tileset;
            missile->tex_c->rect = (Rectangle){.x = 15 * TILE_SIZE, .y = 31 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE};
            missile->life_c = new(missile->life_c);
            missile->life_c->time_remaining = 2.f;
            ecs_entitycontainer_push(world, missile);
        }
        BeginDrawing();

        BeginMode2D(cam);

        ClearBackground(RAYWHITE);

        ecs_entitycontainer_tick(world);

        EndMode2D();

        EndDrawing();
        ecs_entitycontainer_free_queued(world);
    }

    ecs_entitycontainer_free(world);

    UnloadTexture(tileset);
    CloseWindow();        // Close window and OpenGL context
  return 0;
}
