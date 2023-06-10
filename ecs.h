#ifndef ECS_H_
#define ECS_H_

#include <raylib.h>
#include <stdbool.h>
#define MAX_ENTITIES 1024
#define MAX_SYSTEMS 64

// Components
typedef struct {
  Rectangle rect;
  double angle;
  Vector2 origin;
} TransformComponent;

typedef struct {
  Vector2 vel;
} VelocityComponent;

typedef struct {
  Texture2D tex;
  Rectangle rect;
} TextureComponent;

typedef struct {
  float time_remaining;
} LifespanComponent;

typedef struct {
  int hp;
} HealthComponent;

typedef struct {
  Rectangle hitbox;
  unsigned int layer; // bitwise
  unsigned int mask; // bitwise
  int dmg;
  bool break_on_impact;
} CollisionComponent;

typedef struct {
  char text[128]; // hope that's enough lol
} LabelComponent;

typedef struct {
  TransformComponent* trans_c;
  VelocityComponent* vel_c;
  TextureComponent* tex_c;
  LifespanComponent* life_c;
  HealthComponent* hp_c;
  CollisionComponent* col_c;
  LabelComponent* lab_c;
} Entity;

// Entity Container
typedef struct EntityContainer_s {
  Entity* entities[MAX_ENTITIES];
  Entity* queued_for_free[MAX_ENTITIES];
  void (*systems[MAX_SYSTEMS])(struct EntityContainer_s*, Entity*); // Ew, EntityContainer_s?
  Font game_font;
} EntityContainer;

EntityContainer* ecs_entitycontainer_create();
void ecs_entitycontainer_push(EntityContainer* const ec, Entity* const e);
void ecs_entitycontainer_queue_for_freeing(EntityContainer* const ec, Entity* const e);
void ecs_entitycontainer_free_queued(EntityContainer* const ec);
void ecs_entitycontainer_free(EntityContainer* const ec);
void ecs_entitycontainer_add_system(EntityContainer* const ec, void (*system)(EntityContainer*, Entity*));
void ecs_entitycontainer_tick(EntityContainer* const ec);
bool ecs_entitycontainer_contains_entity(EntityContainer const* const ec, Entity const* const e);
bool ecs_entitycontainer_is_entity_queued_for_removal(EntityContainer* const ec, Entity* const e);

// Entity
Entity* ecs_entity_create();
void ecs_entity_free(Entity* const e);

// Systems
void ecs_system_movement(EntityContainer* const ec, Entity* const e);
void ecs_system_render(EntityContainer* const ec, Entity* const e);
void ecs_system_despawn(EntityContainer* const ec, Entity* const e);
void ecs_system_collision(EntityContainer* const ec, Entity* const e);

#endif // ENTITIES_H_
