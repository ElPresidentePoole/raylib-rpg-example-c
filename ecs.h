#ifndef ECS_H_
#define ECS_H_

#include <raylib.h>
#include <stdbool.h>
#include "ecs_components.h"
#include "ecs_systems.h"
#define MAX_ENTITIES 1024
#define MAX_SYSTEMS 16

struct Entity {
  struct TransformComponent* trans_c;
  struct VisibilityComponent* vis_c;
  struct LifespanComponent* life_c;
  struct HealthComponent* hp_c;
  struct CollisionComponent* col_c;
  struct LabelComponent* lab_c;
  struct TimerComponent* tim_c;
  struct ControlComponent* con_c;
  struct InventoryComponent* inv_c;
  struct PickupComponent* pic_c;
  struct TrailComponent* tra_c;
  struct XpRewardComponent* xpr_c;
  struct XpTrackerComponent* xpt_c;
};

// Entity Container
struct EntityContainer {
  struct Entity* entities[MAX_ENTITIES];
  struct Entity* queued_for_free[MAX_ENTITIES];
  void (*systems[MAX_SYSTEMS])(struct EntityContainer* const, struct Entity* const);
  Font game_font;
  Texture2D game_tileset;
  struct Entity const* player; // this entity is very popular, so it would be best if we just held onto it
  Camera2D cam;
};

// Entity Container
struct EntityContainer* ecs_entitycontainer_create();
void ecs_entitycontainer_push(struct EntityContainer* const ec, struct Entity* const e);
void ecs_entitycontainer_queue_for_freeing(struct EntityContainer* const ec, struct Entity* const e);
void ecs_entitycontainer_free_queued(struct EntityContainer* const ec);
void ecs_entitycontainer_free(struct EntityContainer* const ec);
void ecs_entitycontainer_add_system(struct EntityContainer* const ec, void (*system)(struct EntityContainer*, struct Entity*));
void ecs_entitycontainer_tick(struct EntityContainer* const ec);
void ecs_entitycontainer_render(const struct EntityContainer* const ec);
bool ecs_entitycontainer_contains_entity(struct EntityContainer const* const ec, struct Entity const* const e);
bool ecs_entitycontainer_is_entity_queued_for_removal(struct EntityContainer* const ec, struct Entity* const e);

// Entity
struct Entity* ecs_entity_create();
void ecs_entity_free(struct Entity* const e);

// On Timeout Functions
void on_timeout_spawn_troll(struct EntityContainer* const ec, struct Entity* const e);

// Control functions
void e_control_run_towards_player(struct EntityContainer* const ec, struct Entity* const e);
void e_control_player_controls(struct EntityContainer* const ec, struct Entity* const e);

#endif // ENTITIES_H_
