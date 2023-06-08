#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define ECS_DEBUG 0 // draw hitboxes
#include "common_entities.h"
#include "ecs.h"
#include "util.h"

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "rpg");
    // InitAudioDevice();

    Texture2D tileset = LoadTexture("./tileset.png");

    SetTargetFPS(60);

    Camera2D cam = { 0 };
    cam.offset = (Vector2){ screenWidth / 2.f - TILE_SIZE / 2.f, screenHeight / 2.f - TILE_SIZE / 2.f};
    cam.rotation = 0.f;
    cam.zoom = 2.f;


    EntityContainer* world = ecs_entitycontainer_create();
    Entity* player = e_player_create(&tileset, 0, 0);
    ecs_entitycontainer_push(world, player);
    ecs_entitycontainer_push(world, e_troll_create(&tileset, 200, 200));
    ecs_entitycontainer_push(world, e_troll_create(&tileset, -200, -200));
    ecs_entitycontainer_push(world, e_portal_create(&tileset, 200, 0));

    ecs_entitycontainer_add_system(world, &ecs_system_render); // render has to go first to avoid lagging behind
    ecs_entitycontainer_add_system(world, &ecs_system_movement);
    ecs_entitycontainer_add_system(world, &ecs_system_despawn);
    ecs_entitycontainer_add_system(world, &ecs_system_collision);

    const int PLAYER_SPEED = 200; // should this be a component or something?
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
        cam.target = (Vector2){ player->trans_c->rect.x, player->trans_c->rect.y };
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Entity* missile = e_missile_create(&tileset, player, &cam);
            ecs_entitycontainer_push(world, missile);
        }
        BeginDrawing();

        BeginMode2D(cam);

        ClearBackground(BLACK);

        // Draw background
        for(int x = -128; x < 128; x++) {
            for(int y = -128; y < 128; y++) {
                DrawTextureRec(tileset, (Rectangle){.x = 49 * TILE_SIZE, .y = 4 * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE}, (Vector2){.x = x * TILE_SIZE, .y = y * TILE_SIZE}, WHITE);
            }
        }

        #if ECS_DEBUG
        for(int i = 0; i < MAX_ENTITIES; i++) {
            if(world->entities[i] != NULL && world->entities[i]->col_c) DrawRectangleRec(world->entities[i]->col_c->hitbox, GREEN);
        }
        #endif

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
