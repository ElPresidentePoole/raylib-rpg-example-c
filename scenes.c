#include <stdlib.h>
#include "scenes.h"
#include "ecs.h"
#include "common_entities.h"

void scene_main_menu_setup(struct Game* game) {

}

void scene_in_game_setup(struct Game* game) {
  ecs_entitycontainer_add_system(game->world, &ecs_system_movement);
  ecs_entitycontainer_add_system(game->world, &ecs_system_collision);
  ecs_entitycontainer_add_system(game->world, &ecs_system_despawn);
  ecs_entitycontainer_add_system(game->world, &ecs_system_timers);
  ecs_entitycontainer_add_system(game->world, &ecs_system_controls);
  ecs_entitycontainer_add_system(game->world, &ecs_system_experience);
  ecs_entitycontainer_add_system(game->world, &ecs_system_camera);

  struct Entity* player = e_player_create(0, 0);
  game->world->player = player;
  ecs_entitycontainer_push(game->world, player);

  const unsigned int PORTALS = 10;
  for(int i = 0; i < PORTALS; i++) {
    int portal_x = rand() % 800 - 400;
    int portal_y = rand() % 800 - 400;
    ecs_entitycontainer_push(game->world, e_portal_create(portal_x, portal_y));
  }
}

void game_change_scene(struct Game* const game, struct GameScene* new_scene) {
  ecs_entitycontainer_queue_everything_for_freeing(game->world);
  ecs_entitycontainer_rm_all_systems(game->world);
  game->current_scene = new_scene;
}
