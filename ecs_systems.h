#ifndef ECS_SYSTEMS_H_
#define ECS_SYSTEMS_H_

// Systems
void ecs_system_movement(struct EntityContainer* const ec, struct Entity* const e);
void ecs_system_despawn(struct EntityContainer* const ec, struct Entity* const e);
void ecs_system_collision(struct EntityContainer* const ec, struct Entity* const e);
void ecs_system_timers(struct EntityContainer* const ec, struct Entity* const e);
void ecs_system_controls(struct EntityContainer* const ec, struct Entity* const e);
void ecs_system_experience(struct EntityContainer* const ec, struct Entity* const e);
void ecs_system_camera(struct EntityContainer* const ec, struct Entity* const e);

#endif // ECS_SYSTEMS_H_
