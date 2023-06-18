#ifndef SCENE_H_
#define SCENE_H_

//#include "ecs.h"
struct EntityContainer; // forward declaration :)

void scene_main_menu_setup(struct EntityContainer* world);
void scene_in_game_setup(struct EntityContainer* world);

#endif // SCENE_H_
