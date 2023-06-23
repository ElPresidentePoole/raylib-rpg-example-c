#include <stdlib.h>
#include "common_entities.h"
#include "util.h"

void ecs_system_movement(struct EntityContainer* const ec, struct Entity* const e) {
  if (e->trans_c != NULL) {
    float dt = GetFrameTime();
    e->trans_c->position.x += e->trans_c->velocity.x * dt;
    e->trans_c->position.y += e->trans_c->velocity.y * dt;
    e->trans_c->angle += e->trans_c->angular_velocity * dt;
    if(e->col_c != NULL) {
      e->col_c->hitbox.x += e->trans_c->velocity.x * dt;
      e->col_c->hitbox.y += e->trans_c->velocity.y * dt;
    }
  }
  if(e->tra_c != NULL && e->tra_c->remaining_copies > 0) {
    e->tra_c->time_remaining -= GetFrameTime();
    if(e->tra_c->time_remaining <= 0) {
      e->tra_c->time_remaining = e->tra_c->time_between_copies;
      e->tra_c->remaining_copies--;
      struct Entity* ghost = rpg_create_trail_ghost_from_entity(e);
      ecs_entitycontainer_push(ec, ghost);
    }
  }
}

void ecs_system_despawn(struct EntityContainer* const ec, struct Entity* const e) {
  if (e->life_c != NULL) {
    e->life_c->time_remaining -= GetFrameTime();
    if(e->life_c->time_remaining <= 0.f) ecs_entitycontainer_queue_for_freeing(ec, e);
  }
  if (e->hp_c != NULL) {
    if(e->hp_c->hp <= 0) {
      ecs_entitycontainer_queue_for_freeing(ec, e);
      if(e->xpr_c != NULL) {
	      if(ec->player != NULL) ec->player->xpt_c->xp_total += e->xpr_c->xp_reward;
      }
      if(e->inv_c != NULL) {
        struct Entity* coin_drop = rpg_coin_create(e->trans_c->position.x, e->trans_c->position.y, 1);
        ecs_entitycontainer_push(ec, coin_drop);
      }
    }
  }
}

void ecs_system_experience(struct EntityContainer* const ec, struct Entity* const e) {
  if(e->xpt_c != NULL && e->hp_c != NULL) {
    if(e->xpt_c->xp_total >= e->xpt_c->xp_for_next_level) {
      e->xpt_c->level++;
      e->xpt_c->xp_total = 0;
      e->xpt_c->xp_for_next_level *= 2;
      e->hp_c->hp += 10;
    }
  }
}

void ecs_system_collision(struct EntityContainer* const ec, struct Entity* const e) {
  if(e->col_c != NULL) {
    for(int i = 0; i < ec->max_entities; i++) {
      if(ec->entities[i] != NULL && ec->entities[i]->col_c != NULL) {
        if((e->col_c->layer & ec->entities[i]->col_c->mask) > 0 && CheckCollisionRecs(e->col_c->hitbox, ec->entities[i]->col_c->hitbox)) {
          // should it be e->col_c->mask & ec->entities[i]->col_c->layer or e->col_c->layer & ec->entities[i]->col_c->mask ?
          if(e->col_c->break_on_impact) ecs_entitycontainer_queue_for_freeing(ec, e);
          if(e->col_c->dmg > 0 && ec->entities[i]->hp_c != NULL) {
            ec->entities[i]->hp_c->hp -= e->col_c->dmg;
            const char* label_text = TextFormat("%d", e->col_c->dmg);
            struct Entity* label = rpg_label_create(ec->entities[i]->trans_c->position.x, ec->entities[i]->trans_c->position.y, label_text, RED);
            ecs_entitycontainer_push(ec, label);
          }
          if(e->pic_c != NULL && ec->entities[i]->inv_c != NULL) {
            ec->entities[i]->inv_c->gold += e->pic_c->gold_reward;
            struct Entity* label = rpg_label_create(ec->entities[i]->trans_c->position.x, ec->entities[i]->trans_c->position.y, TextFormat("%d", e->pic_c->gold_reward), YELLOW);
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
    if(our_timer->repetitions > 0 || our_timer->repetitions == -1) { // -1 is our magic number for "always repeat"
      our_timer->time_remaining -= GetFrameTime();
      if(our_timer->time_remaining <= 0) {
        our_timer->on_timeout(ec, e);
        our_timer->time_remaining += our_timer->interval;
        if(our_timer->repetitions > 0) our_timer->repetitions--;
      }
    }
  }
}

void ecs_system_controls(struct EntityContainer* const ec, struct Entity* const e) {
  if(e->trans_c != NULL && e->con_c != NULL) {
    e->con_c->control(ec, e);
  }
}

void ecs_system_camera(struct EntityContainer* const ec, struct Entity* const e) {
  if(ec->player != NULL) {
    ec->cam.target.x = ec->player->trans_c->position.x;
    ec->cam.target.y = ec->player->trans_c->position.y;
  } else {
    ec->cam.target = VECTOR2_ZERO;
  }
}

void ecs_system_buttons(struct EntityContainer* const ec, struct Entity* const e) {
  if(e->cli_c != NULL) {
    // if(CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), ec->cam), (Rectangle){e->trans_c->position.x, e->trans_c->position.y, e->cli_c->size.width, e->cli_c->size.height})) {
    if(CheckCollisionPointRec(GetMousePosition(), (Rectangle){e->trans_c->position.x, e->trans_c->position.y, e->cli_c->size.width, e->cli_c->size.height})) {
      // TODO: change color on hover
      if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        e->cli_c->on_click(ec);
      }
    }
  }
}
