#ifndef ECS_H_
#define ECS_H_

#include <raylib.h>
#include <stdbool.h>
#define MAX_ENTITIES 1024
#define MAX_SYSTEMS 16

// Forward declarations
// Component structs -- defined below
struct TransformComponent;
struct VelocityComponent;
struct TextureComponent;
struct LifespanComponent;
struct HealthComponent;
struct CollisionComponent;
struct LabelComponent;
struct TimerComponent;
struct ControlComponent;
struct InventoryComponent;
struct PickupComponent;
struct Entity;
struct EntityContainer;

// Component Definitions
struct TransformComponent {
  Rectangle rect;
  double angle;
  Vector2 origin;
};

struct VelocityComponent {
  Vector2 vel;
};

struct TextureComponent {
  Rectangle rect;
};

struct LifespanComponent {
  float time_remaining;
};

struct HealthComponent {
  int hp;
};

struct CollisionComponent {
  Rectangle hitbox;
  unsigned int layer; // bitwise
  unsigned int mask; // bitwise
  int dmg;
  bool break_on_impact;
};

struct LabelComponent {
  char text[128]; // hope that's enough lol
};

struct TimerComponent {
  bool active;
  bool repeating;
  float time_remaining;
  float interval;
  void (*on_timeout)(struct EntityContainer* const, struct Entity* const);
};

struct ControlComponent {
  void (*control)(struct EntityContainer* const, struct Entity* const);
  // struct Entity const* targeting; // maybe have a controlC and targetC?
};

struct InventoryComponent {
  int gold;
};

struct PickupComponent {
  int gold_reward;
};

struct Entity {
  struct TransformComponent* trans_c;
  struct VelocityComponent* vel_c; // XXX should this just be part of the TransformComponent?
  struct TextureComponent* tex_c;
  struct LifespanComponent* life_c;
  struct HealthComponent* hp_c;
  struct CollisionComponent* col_c;
  struct LabelComponent* lab_c;
  struct TimerComponent* tim_c;
  struct ControlComponent* con_c;
  struct InventoryComponent* inv_c;
  struct PickupComponent* pic_c;
};

// Entity Container
struct EntityContainer {
  struct Entity* entities[MAX_ENTITIES];
  struct Entity* queued_for_free[MAX_ENTITIES];
  void (*systems[MAX_SYSTEMS])(struct EntityContainer* const, struct Entity* const); // Ew, EntityContainer_s?
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

// Systems
void ecs_system_movement(struct EntityContainer* const ec, struct Entity* const e);
void ecs_system_despawn(struct EntityContainer* const ec, struct Entity* const e);
void ecs_system_collision(struct EntityContainer* const ec, struct Entity* const e);
void ecs_system_timers(struct EntityContainer* const ec, struct Entity* const e);
void ecs_system_controls(struct EntityContainer* const ec, struct Entity* const e);

// On Timeout Functions
void on_timeout_spawn_troll(struct EntityContainer* const ec, struct Entity* const e);

// Control functions
void e_control_run_towards_player(struct EntityContainer* const ec, struct Entity* const e);
void e_control_player_controls(struct EntityContainer* const ec, struct Entity* const e);

#endif // ENTITIES_H_
