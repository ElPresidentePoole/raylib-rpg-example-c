#include "rpg_funcs.h"
#include "util.h"
#include "magic.h"
#include "common_entities.h"
#include <stdio.h>

void e_control_run_towards_player(struct EntityContainer* const ec, struct Entity* const e) {
  if(ec->player == NULL) return; // Don't bother if they're dead and gone

  static float cooldown = 2.f;
  cooldown -= GetFrameTime();
  if(get_distance(e->trans_c->position.x, e->trans_c->position.y, ec->player->trans_c->position.x, ec->player->trans_c->position.y) < TILE_SIZE) { // FIXME: shouldn't we be getting the distance based off the pos+origin?
    e->trans_c->velocity = VECTOR2_ZERO;
    if(cooldown <= 0) {
      cooldown = 2.f;
      struct Entity* troll_whack = rpg_hurtbox_create(e->trans_c->position.x, e->trans_c->position.y, 1, LAYER_ENEMY_HURTBOX);
      ecs_entitycontainer_push(ec, troll_whack);
    }
  } else {
    static const int TROLL_SPEED = 100;
    int dx = e->trans_c->position.x - ec->player->trans_c->position.x;
    int dy = e->trans_c->position.y - ec->player->trans_c->position.y;
    double angle_between_troll_and_target = atan2(dy, dx);
    struct Vector2 v2 = (Vector2){-cos(angle_between_troll_and_target), -sin(angle_between_troll_and_target)};
    v2.x *= TROLL_SPEED;
    v2.y *= TROLL_SPEED;
    struct Vector2 pm_normal = Vector2_normalized_multi(v2, TROLL_SPEED);
    e->trans_c->velocity = pm_normal;
  }
}

void e_control_player_controls(struct EntityContainer* const ec, struct Entity* const e) {
  static const int PLAYER_SPEED = 200; // should this be a component or something?
  static const int MELEE_DISTANCE = 32; // should this be a component or something?
  static float cooldown = 0.15f;
  cooldown -= GetFrameTime();
  Vector2 player_movement = (Vector2){.x = 0, .y = 0};
  if(IsKeyDown(KEY_S)) {
    player_movement.y += 1;
  }
  if(IsKeyDown(KEY_W)) {
    player_movement.y -= 1;
  }
  if(IsKeyDown(KEY_D)) {
    player_movement.x += 1;
  }
  if(IsKeyDown(KEY_A)) {
    player_movement.x -= 1;
  }
  struct Vector2 pm_normal = Vector2_normalized_multi(player_movement, PLAYER_SPEED);
  e->trans_c->velocity = pm_normal;

  if(IsMouseButtonDown(MOUSE_LEFT_BUTTON) && cooldown <= 0.f) { 
    if(e->mela_c != NULL) {
      Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), ec->cam);
      int dx = e->trans_c->position.x - mouse_pos.x;
      int dy = e->trans_c->position.y - mouse_pos.y;
      double angle_between_player_and_mouse = atan2(dy, dx);
      Vector2 dp = (Vector2){-cos(angle_between_player_and_mouse), -sin(angle_between_player_and_mouse)};
      struct Entity* hurtbox = rpg_hurtbox_create(e->trans_c->position.x + dp.x * MELEE_DISTANCE, e->trans_c->position.y + dp.y * MELEE_DISTANCE, e->mela_c->dmg, LAYER_MISSILE);
      ecs_entitycontainer_push(ec, hurtbox);
    }
    if(e->rana_c != NULL) {
      Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), ec->cam);
      int dx = e->trans_c->position.x - mouse_pos.x;
      int dy = e->trans_c->position.y - mouse_pos.y;
      double angle_between_player_and_mouse = atan2(dy, dx);
      struct Entity* missile = e->rana_c->missile_func(e->trans_c->position.x, e->trans_c->position.y, angle_between_player_and_mouse, e->rana_c->dmg);
      ecs_entitycontainer_push(ec, missile);
    }
    // struct Entity* missile = rpg_missile_create(e, &ec->cam);
    cooldown = 0.15f;
    // ecs_entitycontainer_push(ec, missile);
  }
}
