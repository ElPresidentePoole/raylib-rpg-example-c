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
  SetWindowState(FLAG_WINDOW_RESIZABLE);

  // GameState current_state = MAIN_MENU;
  // InitAudioDevice();

  struct EntityContainer* world = ecs_entitycontainer_create();
  scene_main_menu_setup(world);
  while (!WindowShouldClose()) {
    ecs_entitycontainer_tick(world);
    ecs_entitycontainer_render(world);
    ecs_entitycontainer_free_queued(world);
  }

  ecs_entitycontainer_free(world);

  CloseWindow();        // Close window and OpenGL context
  return 0;
}
