#include "ecs.h"
#include "util.h"
#include "common_entities.h"
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define ECS_COL_DEBUG 0 // draw hitboxes

struct Entity* ecs_entity_create() {
  struct Entity* e = new(e);
  e->trans_c = NULL;
  e->vel_c = NULL;
  e->tex_c = NULL;
  e->life_c = NULL;
  e->hp_c = NULL;
  e->col_c = NULL;
  e->lab_c = NULL;
  e->tim_c = NULL;
  e->con_c = NULL;
  e->inv_c = NULL;
  e->pic_c = NULL;
  return e;
}

void ecs_entity_free(struct Entity* const e) {
  if (e->trans_c != NULL) free(e->trans_c);
  if (e->vel_c != NULL) free(e->vel_c);
  if (e->tex_c != NULL) free(e->tex_c);
  if (e->life_c != NULL) free(e->life_c);
  if (e->hp_c != NULL) free(e->hp_c);
  if (e->col_c != NULL) free(e->col_c);
  if (e->lab_c != NULL) free(e->lab_c);
  if (e->tim_c != NULL) free(e->tim_c);
  if (e->con_c != NULL) free(e->con_c);
  if (e->inv_c != NULL) free(e->inv_c);
  if (e->pic_c != NULL) free(e->pic_c);
  free(e);
}

void ecs_system_movement(struct EntityContainer* const ec, struct Entity* const e) {
  if (e->trans_c != NULL && e->vel_c != NULL) {
    float dt = GetFrameTime();
    e->trans_c->rect.x += e->vel_c->vel.x * dt;
    e->trans_c->rect.y += e->vel_c->vel.y * dt;
    e->trans_c->angle += e->vel_c->da * dt;
    if(e->col_c != NULL) {
      e->col_c->hitbox.x += e->vel_c->vel.x * dt;
      e->col_c->hitbox.y += e->vel_c->vel.y * dt;
    }
  }
}

void ecs_entitycontainer_render(const struct EntityContainer* const ec) {
  BeginDrawing();
  ClearBackground(BLACK);
  BeginMode2D(ec->cam);

  // Draw background
  for(int x = -128; x < 128; x++) {
    for (int y = -128; y < 128; y++) {
      DrawTextureRec(ec->game_tileset,
                     (Rectangle){.x = 49 * TILE_SIZE,
                                 .y = 4 * TILE_SIZE,
                                 .width = TILE_SIZE,
                                 .height = TILE_SIZE},
                     (Vector2){.x = x * TILE_SIZE, .y = y * TILE_SIZE}, WHITE);
    }
  }

#if ECS_COL_DEBUG
  for (int i = 0; i < MAX_ENTITIES; i++) {
    if (ec->entities[i] != NULL && ec->entities[i]->col_c)
      DrawRectangleRec(ec->entities[i]->col_c->hitbox, GREEN);
  }
#endif

  for (int i = 0; i < MAX_ENTITIES; i++) {
    if (ec->entities[i] != NULL) {
      struct Entity *e = ec->entities[i];
      if (e->trans_c != NULL) {
        if (e->tex_c != NULL) {
          DrawTexturePro(ec->game_tileset, e->tex_c->rect, e->trans_c->rect,
                         e->trans_c->origin, e->trans_c->angle, WHITE);
        }
        if (e->lab_c != NULL) {
          DrawTextEx(
              ec->game_font, e->lab_c->text,
              (Vector2){.x = e->trans_c->rect.x, .y = e->trans_c->rect.y}, 12,
              0.1, e->lab_c->color);
        }
      }
    }
  }

  EndMode2D();

  DrawTextEx(ec->game_font, TextFormat("Gold: %d", ec->player->inv_c->gold), (Vector2){10, 10}, 30.f, 0.1f, YELLOW);
  DrawTextEx(ec->game_font, TextFormat("HP: %d", ec->player->hp_c->hp), (Vector2){10, 40}, 30.f, 0.1f, RED);

  EndDrawing();
}

void ecs_system_despawn(struct EntityContainer* const ec, struct Entity* const e) {
  if (e->life_c != NULL) {
    e->life_c->time_remaining -= GetFrameTime();
    if(e->life_c->time_remaining <= 0.f) ecs_entitycontainer_queue_for_freeing(ec, e);
  }
  if (e->hp_c != NULL) {
    if(e->hp_c->hp <= 0) {
      ecs_entitycontainer_queue_for_freeing(ec, e);
      if(e->inv_c != NULL) {
        struct Entity* coin_drop = e_coin_create(e->trans_c->rect.x, e->trans_c->rect.y, 1);
        ecs_entitycontainer_push(ec, coin_drop);
      }
    }
  }
}

void ecs_system_collision(struct EntityContainer* const ec, struct Entity* const e) {
  if(e->col_c != NULL) {
    for(int i = 0; i < MAX_ENTITIES; i++) {
      if(ec->entities[i] != NULL && ec->entities[i]->col_c != NULL) {
        if((e->col_c->layer & ec->entities[i]->col_c->mask) > 0 && CheckCollisionRecs(e->col_c->hitbox, ec->entities[i]->col_c->hitbox)) {
          // should it be e->col_c->mask & ec->entities[i]->col_c->layer or e->col_c->layer & ec->entities[i]->col_c->mask ?
          if(e->col_c->break_on_impact) ecs_entitycontainer_queue_for_freeing(ec, e);
          if(e->col_c->dmg > 0 && ec->entities[i]->hp_c != NULL) {
            ec->entities[i]->hp_c->hp -= e->col_c->dmg;
            const char* label_text = TextFormat("%d", e->col_c->dmg);
            struct Entity* label = e_label_create(ec->entities[i]->trans_c->rect.x, ec->entities[i]->trans_c->rect.y, label_text, RED);
            ecs_entitycontainer_push(ec, label);
          }
          if(e->pic_c != NULL && ec->entities[i]->inv_c != NULL) {
            ec->entities[i]->inv_c->gold += e->pic_c->gold_reward;
            struct Entity* label = e_label_create(ec->entities[i]->trans_c->rect.x, ec->entities[i]->trans_c->rect.y, TextFormat("%d", e->pic_c->gold_reward), YELLOW);
            ecs_entitycontainer_push(ec, label);
          }
        }
      }
    }
  }
}

void ecs_system_timers(struct EntityContainer* const ec, struct Entity* const e) {
  if(e->tim_c != NULL && e->tim_c->active) {
    struct TimerComponent* our_timer = e->tim_c;
    our_timer->time_remaining -= GetFrameTime();
    if(our_timer->time_remaining <= 0) {
      our_timer->on_timeout(ec, e);
      if(our_timer->repeating) our_timer->time_remaining = our_timer->interval;
    }
  }
}

void ecs_system_controls(struct EntityContainer* const ec, struct Entity* const e) {
  if(e->trans_c != NULL && e->con_c != NULL) {
    e->con_c->control(ec, e);
  }
}

void on_timeout_spawn_troll(struct EntityContainer* const ec, struct Entity* const e) {
  struct Entity* troll = e_troll_create(e->trans_c->rect.x, e->trans_c->rect.y);

  ecs_entitycontainer_push(ec, troll);
}

void ecs_entitycontainer_push(struct EntityContainer* const ec, struct Entity* const e) {
  bool found_spot = false;
  for(int i = 0; i < MAX_ENTITIES && !found_spot; i++) {
    if(ec->entities[i] == NULL) {
      ec->entities[i] = e;
      found_spot = true; // We found a place and don't need to continue any further.
    }
  }

  if(!found_spot) {
    printf("Unable to push entity; couldn't find a free spot.\n");
    printf("Current pointers in ec->entities: ");
    for(int i = 0; i < MAX_ENTITIES; i++) {
      printf(" %p, \n", (void*)ec->entities[i]);
    }
    printf("\ne: %p", (void*)e);
    exit(1);
  }
  // ecs_entity_free(e);
}

void ecs_entitycontainer_queue_for_freeing(struct EntityContainer* const ec, struct Entity* const e) {
  if(!ecs_entitycontainer_is_entity_queued_for_removal(ec, e)) {
    for(int i = 0; i < MAX_ENTITIES; i++) { // XXX: do we need to check if our entities array already contains e?
      if(ec->queued_for_free[i] == NULL) {
        ec->queued_for_free[i] = e;
        return; // We found a place and don't need to continue any further.
      }
    }
    /* printf("Unable to queue entity for removal; couldn't find a free spot.\n"); */
    /* printf("Current pointers in ec->entities: "); */
    /* for(int i = 0; i < MAX_ENTITIES; i++) { */
    /*   printf(" %p, ", (void*)ec->entities[i]); */
    /* } */
    /* printf("\ne: %p", (void*)e); */
    /* exit(1); */
  } else {
    printf("Entity already queued for removal!\n");
  }

}

bool ecs_entitycontainer_is_entity_queued_for_removal(struct EntityContainer* const ec, struct Entity* const e) {
  for(int i = 0; i < MAX_ENTITIES; i++) {
    if(ec->queued_for_free[i] == e) {
      return true;
    }
  }

  return false;
}

static int compare_to_null(const void* p1, const void* p2) {
  if(p1 == NULL && p2 == NULL) return 0;
  else if(p1 == NULL) return -1;
  else if(p2 == NULL) return 1;
  else return 0;
}

void ecs_entitycontainer_free_queued(struct EntityContainer* const ec) {
  for(int ri = 0; ri < MAX_ENTITIES; ri++) { // remove index
    if(ec->queued_for_free[ri] != NULL) { // Don't even try to remove it if it's already NULL!
      struct Entity* to_remove = ec->queued_for_free[ri];
      for(int ei = 0; ei < MAX_ENTITIES; ei++) { // entity index
        if(ec->entities[ei] == to_remove) {
          ecs_entity_free(ec->entities[ei]);
          ec->entities[ei] = NULL;
          ec->queued_for_free[ri] = NULL;
        }
      }
    }
  }

  qsort(ec->entities, MAX_ENTITIES, sizeof(struct Entity*), compare_to_null);
}

bool ecs_entitycontainer_contains_entity(struct EntityContainer const* const ec, struct Entity const* const e) {
  for(int i = 0; i < MAX_ENTITIES; i++) {
    if(ec->entities[i] == e) return true;
  }
  return false;
}


struct EntityContainer* ecs_entitycontainer_create() {
  struct EntityContainer* new_ec = new(new_ec);
  // No dangling pointers here!
  for(int i = 0; i < MAX_ENTITIES; i++) {
    new_ec->entities[i] = NULL;
    new_ec->queued_for_free[i] = NULL;
  }
  for(int i = 0; i < MAX_SYSTEMS; i++) {
    new_ec->systems[i] = NULL;
  }
  new_ec->game_font = LoadFont("./alagard.ttf");
  new_ec->game_tileset = LoadTexture("./tileset.png");
  new_ec->cam = (Camera2D){0};
  new_ec->cam.offset = (Vector2){ SCREEN_WIDTH / 2.f - TILE_SIZE / 2.f, SCREEN_HEIGHT / 2.f - TILE_SIZE / 2.f};
  new_ec->cam.rotation = 0.f;
  new_ec->cam.zoom = 2.f;

  return new_ec;
}

void ecs_entitycontainer_free(struct EntityContainer* const ec) {
  // Free all child entities and then ourselves
  for(int i = 0; i < MAX_ENTITIES; i++) {
    if(ec->entities[i] != NULL) ecs_entity_free(ec->entities[i]);
  }
  UnloadFont(ec->game_font);
  UnloadTexture(ec->game_tileset);
  free(ec);
}

void ecs_entitycontainer_add_system(struct EntityContainer* const ec, void (*system)(struct EntityContainer*, struct Entity*)) {
  for(int i = 0; i < MAX_SYSTEMS; i++) {
    if(ec->systems[i] == NULL) {
      ec->systems[i] = system;
      break;
    }
  }
}

void ecs_entitycontainer_tick(struct EntityContainer* const ec) {
  for (int sys_idx = 0; sys_idx < MAX_SYSTEMS; sys_idx++) {
    if (ec->systems[sys_idx] != NULL) {
      for (int ent_idx = 0; ent_idx < MAX_ENTITIES; ent_idx++) {
        if (ec->entities[ent_idx] != NULL)
          ec->systems[sys_idx](ec, ec->entities[ent_idx]);
      }
    }
  }
  ec->cam.target.x = ec->player->trans_c->rect.x;
  ec->cam.target.y = ec->player->trans_c->rect.y;
}

void e_control_run_towards_player(struct EntityContainer* const ec, struct Entity* const e) {
  static float cooldown = 2.f;
  cooldown -= GetFrameTime();
  if(get_distance(e->trans_c->rect.x, e->trans_c->rect.y, ec->player->trans_c->rect.x, ec->player->trans_c->rect.y) < TILE_SIZE) {
    e->vel_c->vel = VECTOR2_ZERO;
    if(cooldown <= 0) {
      cooldown = 2.f;
      struct Entity* troll_whack = e_hurtbox_create(e->trans_c->rect.x, e->trans_c->rect.y, 1);
      ecs_entitycontainer_push(ec, troll_whack);
    }
  } else {
    static const int TROLL_SPEED = 100;
    int dx = e->trans_c->rect.x - ec->player->trans_c->rect.x;
    int dy = e->trans_c->rect.y - ec->player->trans_c->rect.y;
    double angle_between_troll_and_target = atan2(dy, dx);
    struct Vector2 v2 = (Vector2){-cos(angle_between_troll_and_target), -sin(angle_between_troll_and_target)};
    v2.x *= TROLL_SPEED;
    v2.y *= TROLL_SPEED;
    struct Vector2 pm_normal = Vector2_normalized_multi(v2, TROLL_SPEED);
    e->vel_c->vel = pm_normal;
  }
}

void e_control_player_controls(struct EntityContainer* const ec, struct Entity* const e) {
  static const int PLAYER_SPEED = 200; // should this be a component or something?
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
  e->vel_c->vel = pm_normal;

  if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { 
    struct Entity* missile = e_missile_create(e, &ec->cam);
    ecs_entitycontainer_push(ec, missile);
  }
}
