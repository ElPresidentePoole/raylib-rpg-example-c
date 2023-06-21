#include <stdlib.h>
#include "scenes.h"
#include "ecs.h"
#include "common_entities.h"

void scene_main_menu_setup(struct EntityContainer* world) {
  ecs_entitycontainer_queue_everything_for_freeing(world);
  ecs_entitycontainer_rm_all_systems(world);

  ecs_entitycontainer_add_system(world, &ecs_system_buttons);

  ecs_entitycontainer_push(world, rpg_start_game_button(0, 0));
}

void scene_in_game_setup(struct EntityContainer* world) {
  ecs_entitycontainer_queue_everything_for_freeing(world);
  ecs_entitycontainer_rm_all_systems(world);

  ecs_entitycontainer_add_system(world, &ecs_system_movement);
  ecs_entitycontainer_add_system(world, &ecs_system_collision);
  ecs_entitycontainer_add_system(world, &ecs_system_despawn);
  ecs_entitycontainer_add_system(world, &ecs_system_timers);
  ecs_entitycontainer_add_system(world, &ecs_system_controls);
  ecs_entitycontainer_add_system(world, &ecs_system_experience);
  ecs_entitycontainer_add_system(world, &ecs_system_camera);

  struct Entity* player = rpg_player_create(0, 0);
  world->player = player;
  ecs_entitycontainer_push(world, player);

  const unsigned int PORTALS = 10;
  for(int i = 0; i < PORTALS; i++) {
    int portal_x = rand() % 800 - 400;
    int portal_y = rand() % 800 - 400;
    ecs_entitycontainer_push(world, rpg_portal_create(portal_x, portal_y));
  }
}
