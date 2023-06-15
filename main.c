#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "common_entities.h"
#include "ecs.h"
#include "util.h"

typedef enum {
	MAIN_MENU
} GameState;

int main() {
    srand(time(NULL));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "rpg");

    // GameState current_state = MAIN_MENU;
    // InitAudioDevice();

    SetTargetFPS(60); // TODO: replace this with proper handling of DT

    struct EntityContainer* world = ecs_entitycontainer_create();
    struct Entity* player = e_player_create(0, 0);
    world->player = player;
    ecs_entitycontainer_push(world, player);
    ecs_entitycontainer_push(world, e_portal_create(200, 0));

    ecs_entitycontainer_add_system(world, &ecs_system_movement);
    ecs_entitycontainer_add_system(world, &ecs_system_collision);
    ecs_entitycontainer_add_system(world, &ecs_system_despawn);
    ecs_entitycontainer_add_system(world, &ecs_system_timers);
    ecs_entitycontainer_add_system(world, &ecs_system_controls);
    ecs_entitycontainer_add_system(world, &ecs_system_experience);

    while (!WindowShouldClose()) {
        ecs_entitycontainer_tick(world);
        ecs_entitycontainer_render(world);
        ecs_entitycontainer_free_queued(world);
    }

    ecs_entitycontainer_free(world);

    CloseWindow();        // Close window and OpenGL context
  return 0;
}
