#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "common_entities.h"
#include "ecs.h"
#include "util.h"
#include "scenes.h"

/*
#define PORTAL_COUNT 10
void randomly_place_portal(struct EntityContainer* world) {
  int portal_x = rand() % 800 - 400;
  int portal_y = rand() % 800 - 400;
  ecs_entitycontainer_push(world, e_portal_create(portal_x, portal_y));
}*/

int main() {
  srand(time(NULL));

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "rpg");

  // GameState current_state = MAIN_MENU;
  // InitAudioDevice();

  SetTargetFPS(60); // TODO: replace this with proper handling of DT

  // struct GameScene main_menu_scene = (struct GameScene){ &scene_main_menu_setup };
  struct GameScene in_game_scene = (struct GameScene){ &scene_in_game_setup };

  struct Game g = { ecs_entitycontainer_create(), &in_game_scene };
  g.current_scene->setup(&g);

  // struct EntityContainer* world = ecs_entitycontainer_create();
  // for(int p = 0; p < PORTAL_COUNT; p++) {
    // randomly_place_portal(world);
  // }
  // ecs_entitycontainer_push(world, player);

  // ecs_entitycontainer_add_system(world, &ecs_system_movement);
  // ecs_entitycontainer_add_system(world, &ecs_system_collision);
  // ecs_entitycontainer_add_system(world, &ecs_system_despawn);
  // ecs_entitycontainer_add_system(world, &ecs_system_timers);
  // ecs_entitycontainer_add_system(world, &ecs_system_controls);
  // ecs_entitycontainer_add_system(world, &ecs_system_experience);
  // ecs_entitycontainer_add_system(world, &ecs_system_camera);

  while (!WindowShouldClose()) {
    ecs_entitycontainer_tick(g.world);
    ecs_entitycontainer_render(g.world);
    ecs_entitycontainer_free_queued(g.world);
  }

  // ecs_entitycontainer_free(world);

  CloseWindow();        // Close window and OpenGL context
  return 0;
}
