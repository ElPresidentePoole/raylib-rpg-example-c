#ifndef RPG_FUNCS_H_
#define RPG_FUNCS_H_
#include "ecs.h"

void e_control_run_towards_player(struct EntityContainer* const ec, struct Entity* const e);
void e_control_player_controls(struct EntityContainer* const ec, struct Entity* const e);

#endif // RPG_FUNCS_H_
