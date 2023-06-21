#ifndef ECS_COMPONENTS_H_
#define ECS_COMPONENTS_H_
#include <raylib.h>
#include "ecs_structs.h"
// Forward declarations
// Component structs -- defined below
struct TransformComponent;
struct TextureComponent;
struct VisibilityComponent;
struct LifespanComponent;
struct HealthComponent;
struct CollisionComponent;
struct LabelComponent;
struct TimerComponent;
struct ControlComponent;
struct InventoryComponent;
struct PickupComponent;
struct TrailComponent;
struct XpRewardComponent;
struct XpTrackerComponent;
struct ClickableComponent;
struct CameraFollowComponent; // TODO: not implemented :)
struct Entity;
struct EntityContainer;

// Component Definitions
struct TransformComponent {
  Vector2 position;
  double angle;
  Vector2 origin;
  Vector2 velocity;
  double angular_velocity;
};

struct TextureComponent {
  Rectangle source;
};

struct VisibilityComponent {
  unsigned char alpha;
  bool fading;
  int fade_per_second;
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
  Color color;
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

struct TrailComponent {
  float time_between_copies;
  float time_remaining;
  unsigned int remaining_copies;
};

struct XpRewardComponent {
	unsigned int xp_reward;
};

struct XpTrackerComponent {
	unsigned int xp_total;
	unsigned int xp_for_next_level;
	unsigned int level;
};

struct ClickableComponent {
  void (*on_click)(struct EntityContainer* const);
  struct Dimensions size;
};

#endif // ECS_COMPONENTS_H_
