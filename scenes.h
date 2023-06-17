#ifndef SCENE_H_
#define SCENE_H_

#include "ecs.h"

struct GameScene;
struct Game;

struct GameScene {
  void (*setup)(struct Game* game);
};

void scene_main_menu_setup(struct Game* game);
void scene_in_game_setup(struct Game* game);

struct Game {
  struct EntityContainer* world;
  struct GameScene* current_scene;
};

void game_change_scene(struct Game* const game, struct GameScene* new_scene);

#endif // SCENE_H_
