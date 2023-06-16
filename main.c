#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "common_entities.h"
#include "ecs.h"
#include "util.h"

// typedef enum {
  // IN_GAME
// } GameState;

#define PORTAL_COUNT 10
void randomly_place_portal(struct EntityContainer* world) {
  int portal_x = rand() % 800 - 400;
  int portal_y = rand() % 800 - 400;
  ecs_entitycontainer_push(world, e_portal_create(portal_x, portal_y));
}

int main() {
  srand(time(NULL));

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "rpg");

  // GameState current_state = MAIN_MENU;
  // InitAudioDevice();

  SetTargetFPS(60); // TODO: replace this with proper handling of DT

  struct EntityContainer* world = ecs_entitycontainer_create();
  for(int p = 0; p < PORTAL_COUNT; p++) {
    randomly_place_portal(world);
  }
  struct Entity* player = e_player_create(0, 0);
  world->player = player;
  ecs_entitycontainer_push(world, player);

  ecs_entitycontainer_add_system(world, &ecs_system_movement);
  ecs_entitycontainer_add_system(world, &ecs_system_collision);
  ecs_entitycontainer_add_system(world, &ecs_system_despawn);
  ecs_entitycontainer_add_system(world, &ecs_system_timers);
  ecs_entitycontainer_add_system(world, &ecs_system_controls);
  ecs_entitycontainer_add_system(world, &ecs_system_experience);
  ecs_entitycontainer_add_system(world, &ecs_system_camera);

  while (!WindowShouldClose()) {
    ecs_entitycontainer_tick(world);
    ecs_entitycontainer_render(world);
    ecs_entitycontainer_free_queued(world);
  }

  ecs_entitycontainer_free(world);

  CloseWindow();        // Close window and OpenGL context
  return 0;
}
