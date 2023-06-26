#include "ecs.h"
#include "util.h"
#include "magic.h"
#include "common_entities.h"
#include <raylib.h>
#include <sys/param.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define ECS_COL_DEBUG 0 // draw hitboxes

struct Entity* ecs_entity_create() {
  struct Entity* e = new(e);
  e->trans_c = NULL;
  e->tex_c = NULL;
  e->alp_c = NULL;
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
  e->cli_c = NULL;
  e->camf_c = NULL;
  e->mela_c = NULL;
  e->rana_c = NULL;
  return e;
}

void ecs_entity_free(struct Entity* const e) {
  if (e->trans_c != NULL) free(e->trans_c);
  if (e->tex_c != NULL) free(e->tex_c);
  if (e->alp_c != NULL) free(e->alp_c);
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
  if (e->cli_c != NULL) free(e->cli_c);
  if (e->camf_c != NULL) free(e->camf_c);
  if (e->mela_c != NULL) free(e->mela_c);
  if (e->rana_c != NULL) free(e->rana_c);
  free(e);
}


void ecs_entitycontainer_render(const struct EntityContainer* const ec) {
  BeginDrawing();
  BeginTextureMode(ec->render_here);
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
  for (int i = 0; i < ec->max_entities; i++) {
    if (ec->entities[i] != NULL && ec->entities[i]->col_c)
      DrawRectangleRec(ec->entities[i]->col_c->hitbox, GREEN);
  }
#endif

  for (int i = 0; i < ec->max_entities; i++) {
    if (ec->entities[i] != NULL) {
      struct Entity* e = ec->entities[i];
      if(e->trans_c != NULL && e->trans_c->uses_world_position) {
        unsigned char draw_alpha = 255U;
        if(e->alp_c != NULL && e->alp_c->fading) {
          e->alp_c->alpha = MAX(e->alp_c->alpha - e->alp_c->fade_per_second * GetFrameTime(), 0);
          draw_alpha = e->alp_c->alpha;
        }
        if(e->tex_c != NULL) {
          Color white_transparent = {255, 255, 255, draw_alpha};
          Rectangle dest = (Rectangle){ e->trans_c->position.x, e->trans_c->position.y, TILE_SIZE, TILE_SIZE };
          DrawTexturePro(ec->game_tileset, e->tex_c->source, dest, e->trans_c->origin, e->trans_c->angle, white_transparent);
        }

        if(e->lab_c != NULL) {
          Color label_color_with_alpha = (Color){ e->lab_c->color.r, e->lab_c->color.g, e->lab_c->color.b, draw_alpha};
          Color black_with_alpha = (Color){ BLACK.r, BLACK.g, BLACK.b, draw_alpha };
          draw_text_with_bg(ec->game_font, e->lab_c->text, e->trans_c->position, e->lab_c->size, 0.1f, label_color_with_alpha, black_with_alpha);
        }
      }
    }
  }

  EndMode2D();
  if(ec->player != NULL) {
    draw_text_with_bg(ec->game_font, TextFormat("Level: %d | XP: %d/%d", ec->player->xpt_c->level, ec->player->xpt_c->xp_total, ec->player->xpt_c->xp_for_next_level), (Vector2){10, 10}, 30.f, 0.1f, BLUE, DARKBLUE);
    draw_text_with_bg(ec->game_font, TextFormat("Gold: %d", ec->player->inv_c->gold), (Vector2){10, 40}, 30.f, 0.1f, YELLOW, GOLD);
    draw_text_with_bg(ec->game_font, TextFormat("HP: %d", ec->player->hp_c->hp), (Vector2){10, 70}, 30.f, 0.1f, RED, BROWN);
  } // else {
    //DrawTextEx(ec->game_font, "Game Over", (Vector2) { 20, 20 }, 70.f, 0.1f, RED);
  //}
  for (int i = 0; i < ec->max_entities; i++) {
    if (ec->entities[i] != NULL) {
      struct Entity* e = ec->entities[i];
      if(e->trans_c != NULL && !e->trans_c->uses_world_position) {
        unsigned char draw_alpha = 255U;
        if(e->cli_c != NULL) {
          Rectangle button_rect = (Rectangle){e->trans_c->position.x, e->trans_c->position.y, e->cli_c->size.width, e->cli_c->size.height};
          DrawRectangleRec(button_rect, GREEN);
        }
        if(e->lab_c != NULL) {
          Color fg_color = (Color){e->lab_c->color.r, e->lab_c->color.g, e->lab_c->color.b, draw_alpha};
          Color bg_color = (Color){BLACK.r, BLACK.g, BLACK.b, draw_alpha};
          draw_text_with_bg(ec->game_font, e->lab_c->text, e->trans_c->position, e->lab_c->size, 0.1f, fg_color, bg_color);
        }
      }
    }
  }

  DrawFPS(12, 12);
  EndTextureMode();
  DrawTexturePro(ec->render_here.texture, (Rectangle){0, 0, SCREEN_WIDTH, -SCREEN_HEIGHT}, (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()}, VECTOR2_ZERO, 0.f, WHITE);
  EndDrawing();
}

void on_timeout_spawn_troll(struct EntityContainer* const ec, struct Entity* const e) {
  struct Entity* troll = rpg_troll_create(e->trans_c->position.x, e->trans_c->position.y);

  ecs_entitycontainer_push(ec, troll);
  // ecs_entitycontainer_queue_for_freeing(ec, e);
}

void on_timeout_manage_wave(struct EntityContainer* const ec, struct Entity* const e) {
  printf("TODO on_timeout_manage_wave\n");
  const char* t = TextFormat("Wave: %d", 1);
  strcpy(e->lab_c->text, t);
}

void ecs_entitycontainer_push(struct EntityContainer* const ec, struct Entity* const e) {
  bool found_spot = false;
  for(int i = 0; i < ec->max_entities && !found_spot; i++) {
    if(ec->entities[i] == NULL) {
      ec->entities[i] = e;
      found_spot = true; // We found a place and don't need to continue any further.
    }
  }

  if(!found_spot) {
    // unsigned long previous_size = ec->max_entities;
    unsigned long old_size = ec->max_entities;
    unsigned long new_size = ec->max_entities * 2;
    struct Entity** new_entities = realloc(ec->entities, new_size * sizeof(struct Entity*));
    if(new_entities == NULL) {
      printf("Unable to push entity; couldn't realloc more memory for entities.\n");
      exit(1);
    }
    // We just allocated (potential) garbage.  Let's clean up!
    for(unsigned long i = ec->max_entities; i < new_size; i++) {
      new_entities[i] = NULL;
    }

    struct Entity** new_queued_for_free = realloc(ec->queued_for_free, new_size * sizeof(struct Entity*));
    if(new_queued_for_free == NULL) {
      printf("Unable to push entity; couldn't realloc more memory for queued_for_free.\n");
      exit(1);
    }
    // We just allocated (potential) garbage.  Let's clean up!
    for(unsigned long i = ec->max_entities; i < new_size; i++) {
      new_queued_for_free[i] = NULL;
    }

    ec->entities = new_entities;
    ec->queued_for_free = new_queued_for_free;
    ec->max_entities = new_size;
    printf("Successfully resized ec->entities to %lu\n", ec->max_entities);
    ec->entities[old_size] = e;
    /*
    printf("Current pointers in ec->entities: ");
    for(int i = 0; i < ec->max_entities; i++) {
      printf(" %p, \n", (void*)ec->entities[i]);
    }
    printf("\ne: %p", (void*)e);
    exit(1);*/
  }
  // ecs_entity_free(e);
}

void ecs_entitycontainer_queue_for_freeing(struct EntityContainer* const ec, struct Entity* const e) {
  if(!ecs_entitycontainer_is_entity_queued_for_removal(ec, e)) {
    for(int i = 0; i < ec->max_entities; i++) { // XXX: do we need to check if our entities array already contains e?
      if(ec->queued_for_free[i] == NULL) {
        ec->queued_for_free[i] = e;
        return; // We found a place and don't need to continue any further.
      }
    }
    /* printf("Unable to queue entity for removal; couldn't find a free spot.\n"); */
    /* printf("Current pointers in ec->entities: "); */
    /* for(int i = 0; i < ec->max_entities; i++) { */
    /*   printf(" %p, ", (void*)ec->entities[i]); */
    /* } */
    /* printf("\ne: %p", (void*)e); */
    /* exit(1); */
  } else {
    printf("Entity already queued for removal!\n");
  }

}

void ecs_entitycontainer_queue_everything_for_freeing(struct EntityContainer* const ec) {
  for(int i = 0; i < ec->max_entities; i++) {
    if(ec->entities[i] != NULL) {
      ecs_entitycontainer_queue_for_freeing(ec, ec->entities[i]);
    }
  }
}

bool ecs_entitycontainer_is_entity_queued_for_removal(struct EntityContainer* const ec, struct Entity* const e) {
  for(int i = 0; i < ec->max_entities; i++) {
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
  for(int ri = 0; ri < ec->max_entities; ri++) { // remove index
    if(ec->queued_for_free[ri] != NULL) { // Don't even try to remove it if it's already NULL!
      struct Entity* to_remove = ec->queued_for_free[ri];
      if (to_remove == ec->player) ec->player = NULL; // special case for the player
      for(int ei = 0; ei < ec->max_entities; ei++) { // entity index
        if(ec->entities[ei] == to_remove) {
          ecs_entity_free(ec->entities[ei]);
          ec->entities[ei] = NULL;
          ec->queued_for_free[ri] = NULL;
        }
      }
    }
  }

  qsort(ec->entities, ec->max_entities, sizeof(struct Entity*), compare_to_null);
}

bool ecs_entitycontainer_contains_entity(struct EntityContainer const* const ec, struct Entity const* const e) {
  for(int i = 0; i < ec->max_entities; i++) {
    if(ec->entities[i] == e) return true;
  }
  return false;
}


struct EntityContainer* ecs_entitycontainer_create() {
  struct EntityContainer* new_ec = new(new_ec);
  new_ec->max_entities = MAX_ENTITIES_INITIAL;
  new_ec->entities = malloc(sizeof(struct Entity*) * new_ec->max_entities);
  new_ec->queued_for_free = malloc(sizeof(struct Entity*) * new_ec->max_entities);
  // No dangling pointers here!
  for(int i = 0; i < new_ec->max_entities; i++) {
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
  new_ec->player = NULL;
  new_ec->render_here = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

  return new_ec;
}

void ecs_entitycontainer_free(struct EntityContainer* const ec) {
  // Free all child entities and then ourselves
  for(int i = 0; i < ec->max_entities; i++) {
    if(ec->entities[i] != NULL) ecs_entity_free(ec->entities[i]);
    if(ec->queued_for_free[i] != NULL) ecs_entity_free(ec->queued_for_free[i]); // neccessary?
  }
  UnloadFont(ec->game_font);
  UnloadTexture(ec->game_tileset);
  free(ec->entities);
  free(ec->queued_for_free);
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

void ecs_entitycontainer_rm_all_systems(struct EntityContainer* const ec) {
  for(int i = 0; i < MAX_SYSTEMS; i++) {
    if(ec->systems[i] != NULL) {
      ec->systems[i] = NULL;
    }
  }
}

void ecs_entitycontainer_tick(struct EntityContainer* const ec) {
  for (int sys_idx = 0; sys_idx < MAX_SYSTEMS; sys_idx++) {
    if (ec->systems[sys_idx] != NULL) {
      for (int ent_idx = 0; ent_idx < ec->max_entities; ent_idx++) {
        if (ec->entities[ent_idx] != NULL)
          ec->systems[sys_idx](ec, ec->entities[ent_idx]);
      }
    }
  }
}

