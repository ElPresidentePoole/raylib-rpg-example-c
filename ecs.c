#include "ecs.h"
#include "util.h"
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

Entity* ecs_entity_create() {
  Entity* e = new(e);
  e->trans_c = NULL;
  e->vel_c = NULL;
  e->tex_c = NULL;
  e->life_c = NULL;
  // e->hp_c = NULL;
  e->col_c = NULL;
  return e;
}

void ecs_entity_free(Entity* const e) {
  if (e->trans_c != NULL) free(e->trans_c);
  if (e->vel_c != NULL) free(e->vel_c);
  if (e->tex_c != NULL) free(e->tex_c);
  if (e->life_c != NULL) free(e->life_c);
  // if (e->hp_c != NULL) free(e->hp_c);
  if (e->col_c != NULL) free(e->col_c);
  free(e);
}

void ecs_system_movement(EntityContainer* const ec, Entity* const e) {
    if (e->trans_c != NULL && e->vel_c != NULL) {
      float dt = GetFrameTime();
      e->trans_c->rect.x += e->vel_c->vel.x * dt;
      e->trans_c->rect.y += e->vel_c->vel.y * dt;
      if(e->col_c != NULL) {
        e->col_c->hitbox.x += e->vel_c->vel.x * dt;
        e->col_c->hitbox.y += e->vel_c->vel.y * dt;
      }
    }
}

void ecs_system_render(EntityContainer* const ec, Entity* const e) {
    if (e->trans_c != NULL && e->tex_c != NULL) {
      DrawTexturePro(e->tex_c->tex, e->tex_c->rect, e->trans_c->rect, e->trans_c->origin, e->trans_c->angle, WHITE);
    }
}

void ecs_system_despawn(EntityContainer* const ec, Entity* const e) {
    if (e->life_c != NULL) {
        e->life_c->time_remaining -= GetFrameTime();
        if(e->life_c->time_remaining <= 0.f) ecs_entitycontainer_queue_for_freeing(ec, e);
    }
}

void ecs_system_collision(EntityContainer* const ec, Entity* const e) {
  if(e->col_c != NULL) {
    for(int i = 0; i < MAX_ENTITIES; i++) {
      if(ec->entities[i] != NULL && ec->entities[i]->col_c != NULL) {
        if((e->col_c->mask & ec->entities[i]->col_c->layer) > 0 && CheckCollisionRecs(e->col_c->hitbox, ec->entities[i]->col_c->hitbox)) {
          // should it be e->col_c->mask & ec->entities[i]->col_c->layer or e->col_c->layer & ec->entities[i]->col_c->mask
          ecs_entitycontainer_queue_for_freeing(ec, e);
        }
      }
    }
  }
}

void ecs_entitycontainer_push(EntityContainer* const ec, Entity* const e) {
  for(int i = 0; i < MAX_ENTITIES; i++) {
    if(ec->entities[i] == NULL) {
      ec->entities[i] = e;
      return; // We found a place and don't need to continue any further.
    }
  }

  printf("Unable to push entity; couldn't find a free spot.\n");
  ecs_entity_free(e);
  //exit(1);
}

void ecs_entitycontainer_queue_for_freeing(EntityContainer* const ec, Entity* const e) {
  if(!ecs_entitycontainer_is_entity_queued_for_removal(ec, e)) {
    for(int i = 0; i < MAX_ENTITIES; i++) { // XXX: do we need to check if our entities array already contains e?
      if(ec->queued_for_free[i] == NULL) {
        ec->queued_for_free[i] = e;
        return; // We found a place and don't need to continue any further.
      }
    }
  } else {
    printf("Entity already queued for removal!\n");
  }

  printf("Unable to queue entity for removal; couldn't find a free spot.\n");
  exit(1);
}

bool ecs_entitycontainer_is_entity_queued_for_removal(EntityContainer* const ec, Entity* const e) {
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

void ecs_entitycontainer_free_queued(EntityContainer* const ec) {
  for(int ri = 0; ri < MAX_ENTITIES; ri++) { // remove index
    if(ec->queued_for_free[ri] != NULL) { // Don't even try to remove it if it's already NULL!
      Entity* to_remove = ec->queued_for_free[ri];
      for(int ei = 0; ei < MAX_ENTITIES; ei++) { // entity index
        if(ec->entities[ei] == to_remove) {
          ecs_entity_free(ec->entities[ei]);
          ec->entities[ei] = NULL;
          ec->queued_for_free[ri] = NULL;
        }
      }
    }
  }

  qsort(ec->entities, MAX_ENTITIES, sizeof(Entity*), compare_to_null);
}

bool ecs_entitycontainer_contains_entity(EntityContainer const* const ec, Entity const* const e) {
  for(int i = 0; i < MAX_ENTITIES; i++) {
    if(ec->entities[i] == e) return true;
  }
  return false;
}


EntityContainer* ecs_entitycontainer_create() {
  EntityContainer* new_ec = new(new_ec);
  // No dangling pointers here!
  for(int i = 0; i < MAX_ENTITIES; i++) {
    new_ec->entities[i] = NULL;
    new_ec->queued_for_free[i] = NULL;
  }
  for(int i = 0; i < MAX_SYSTEMS; i++) {
    new_ec->systems[i] = NULL;
  }
  return new_ec;
}

void ecs_entitycontainer_free(EntityContainer* const ec) {
  // Free all child entities and then ourselves
  for(int i = 0; i < MAX_ENTITIES; i++) {
    if(ec->entities[i] != NULL) ecs_entity_free(ec->entities[i]);
  }
  free(ec);
}

void ecs_entitycontainer_add_system(EntityContainer* const ec, void (*system)(EntityContainer*, Entity*)) {
  for(int i = 0; i < MAX_SYSTEMS; i++) {
    if(ec->systems[i] == NULL) {
      ec->systems[i] = system;
      break;
    }
  }
}

void ecs_entitycontainer_tick(EntityContainer* const ec) {
  for (int sys_idx = 0; sys_idx < MAX_SYSTEMS; sys_idx++) {
    if (ec->systems[sys_idx] != NULL) {
      for (int ent_idx = 0; ent_idx < MAX_ENTITIES; ent_idx++) {
        if (ec->entities[ent_idx] != NULL)
          ec->systems[sys_idx](ec, ec->entities[ent_idx]);
      }
    }
  }
}
