#include "ecs.h"
#include "util.h"
#include "common_entities.h"
#include <raylib.h>
#include <sys/param.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#define ECS_COL_DEBUG 0 // draw hitboxes

struct Entity* ecs_entity_create() {
  struct Entity* e = new(e);
  e->trans_c = NULL;
  e->vis_c = NULL;
  e->life_c = NULL;
  e->hp_c = NULL;
  e->col_c = NULL;
  e->lab_c = NULL;
  e->tim_c = NULL;
  e->con_c = NULL;
  e->inv_c = NULL;
  e->pic_c = NULL;
  e->tra_c = NULL;
  e->xpr_c = NULL;
  e->xpt_c = NULL;
  return e;
}

void ecs_entity_free(struct Entity* const e) {
  if (e->trans_c != NULL) free(e->trans_c);
  if (e->vis_c != NULL) free(e->vis_c);
  if (e->life_c != NULL) free(e->life_c);
  if (e->hp_c != NULL) free(e->hp_c);
  if (e->col_c != NULL) free(e->col_c);
  if (e->lab_c != NULL) free(e->lab_c);
  if (e->tim_c != NULL) free(e->tim_c);
  if (e->con_c != NULL) free(e->con_c);
  if (e->inv_c != NULL) free(e->inv_c);
  if (e->pic_c != NULL) free(e->pic_c);
  if (e->tra_c != NULL) free(e->tra_c);
  if (e->xpr_c != NULL) free(e->xpr_c);
  if (e->xpt_c != NULL) free(e->xpt_c);
  free(e);
}


void ecs_entitycontainer_render(const struct EntityContainer* const ec) {
  BeginDrawing();
  ClearBackground(BLACK);
  BeginMode2D(ec->cam);

  // Draw background
  for(int x = -128; x < 128; x++) {
    for (int y = -128; y < 128; y++) {
      int tile_x = 19; // math kurduggery to make a checkered floor
      int tile_y = 5;
      if(x % 2 == 0) tile_x--;
      if(y % 2 == 0) tile_x--;
      DrawTextureRec(ec->game_tileset,
                     (Rectangle){.x = tile_x * TILE_SIZE,
                                 .y = tile_y * TILE_SIZE,
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
        if (e->vis_c != NULL) {
          if (e->vis_c->fading) e->vis_c->alpha = MAX(e->vis_c->alpha - e->vis_c->fade_per_second * GetFrameTime(), 0);
          Color c = {255, 255, 255, e->vis_c->alpha};
          DrawTexturePro(ec->game_tileset, e->vis_c->rect, e->trans_c->rect,
                         e->trans_c->origin, e->trans_c->angle, c);
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
  DrawTextEx(ec->game_font, TextFormat("Level: %d | XP: %d/%d", ec->player->xpt_c->level, ec->player->xpt_c->xp_total, ec->player->xpt_c->xp_for_next_level), (Vector2){12, 12}, 30.f, 0.1f, DARKBLUE);
  DrawTextEx(ec->game_font, TextFormat("Level: %d | XP: %d/%d", ec->player->xpt_c->level, ec->player->xpt_c->xp_total, ec->player->xpt_c->xp_for_next_level), (Vector2){10, 10}, 30.f, 0.1f, BLUE);
  DrawTextEx(ec->game_font, TextFormat("Gold: %d", ec->player->inv_c->gold), (Vector2){11, 41}, 30.f, 0.1f, GOLD);
  DrawTextEx(ec->game_font, TextFormat("Gold: %d", ec->player->inv_c->gold), (Vector2){10, 40}, 30.f, 0.1f, YELLOW);
  DrawTextEx(ec->game_font, TextFormat("HP: %d", ec->player->hp_c->hp), (Vector2){11, 71}, 30.f, 0.1f, BROWN);
  DrawTextEx(ec->game_font, TextFormat("HP: %d", ec->player->hp_c->hp), (Vector2){10, 70}, 30.f, 0.1f, RED);

  EndDrawing();
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
    e->trans_c->velocity = VECTOR2_ZERO;
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
    e->trans_c->velocity = pm_normal;
  }
}

void e_control_player_controls(struct EntityContainer* const ec, struct Entity* const e) {
  static const int PLAYER_SPEED = 200; // should this be a component or something?
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
    struct Entity* missile = e_missile_create(e, &ec->cam);
    cooldown = 0.15f;
    ecs_entitycontainer_push(ec, missile);
  }
}
