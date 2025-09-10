#include "gs_update.h"

#include <stdlib.h>
#include <string.h>

#include "camera_util.h"
#include "shape_util.h"

void GsSpawnNewShapes(GameScene* GS) {
  int shape_limit = 25 + GS->player.level;
  if (GS->ticks % 2 == 0 && GS->shape_count < (LENGTHOF(GS->shapes) / 2) && GS->shape_count < shape_limit) {
    fixed_t new_x = GS->player.x + fixed_new(GetRandomValue(-render_w, render_w), 0);
    fixed_t new_y = GS->player.y + fixed_new(GetRandomValue(-render_h, render_h), 0);
    if (fixed_abs(new_x - GS->player.x) / fixed_factor < render_w / 2 &&
        fixed_abs(new_y - GS->player.y) / fixed_factor < render_h / 2) {
      return;
    }
    int s = ClaimEmptyShapeSlot(GS);
    if (s == -1) {
      return;
    }
    GS->shapes[s].x = new_x;
    GS->shapes[s].y = new_y;
    GS->shapes[s].move_angle = GetRandomValue(0, angle_factor - 1);
    GS->shapes[s].ticks_since_damaged = 1000;
    GS->shapes[s].sides = PickShapeSides(GS);
    GS->shapes[s].variant = PickShapeVariant(GS, GS->shapes[s].sides);
    SetShapeStats(GS, s);
    // printf("Spawned shape %d\n", s);
  }
}

void GsSpawnPickup(GameScene* GS, fixed_t x, fixed_t y) {
  for (int p = 0; p < LENGTHOF(GS->pickups); ++p) {
    if (GS->pickups[p].exists) {
      continue;
    }
    GS->pickups[p].exists = true;
    GS->pickups[p].x = x;
    GS->pickups[p].y = y;
    GS->pickups[p].type = GetRandomValue(0, ITEM_COUNT - 1);
    if (GS->player.item_counts[GS->pickups[p].type] >= 8) {
      GS->pickups[p].exists = false;
      return;
    }
    ++GS->pickups_spawned;
    return;
  }
}

void GsUpdatePlayerStats(GameScene* GS, GsPlayerStats* stats) {
  stats->max_hp = 400;
  stats->active_regen_delay = 300;
  stats->shot_damage = 40;
  stats->max_move_speed = fixed_new(1, 128);
  stats->reload_delay = fixed_new(0, 64);
  stats->shot_count = fixed_new(1, 0);
  stats->shot_kb = fixed_new(0, 0);
  stats->shot_pierce = 1;
  stats->shot_speed = fixed_new(2, 0);
  stats->shot_spread = 1;
  stats->sight_range = render_h / 2;
  stats->size = 12;
  stats->turn_speed = 2;
  stats->magnetism_dist = 16;
  stats->magnetism_percent = 0;
  stats->shot_homing_percent = 0;
  stats->shot_homing_power = 0;
  stats->view_distance = 120;
  stats->contact_damage = 20;
  stats->active_regen = 20;
  stats->passive_regen = 0;
  stats->creativity = 0;
  stats->shot_split_fragments = 2;
  stats->shot_split_percent = 0;
  for (int i = 0; i < ITEM_COUNT; ++i) {
    for (int x = 0; x < GS->player.item_counts[i]; ++x) {
      switch (i) {
        case ITEM_SPEED_UP:
          stats->max_move_speed += fixed_new(0, 96);
          break;
        case ITEM_FIRE_RATE_UP:
          stats->reload_delay = stats->reload_delay * 80 / 100;
          stats->shot_spread += 3;
          break;
        case ITEM_TURN_SPEED_UP:
          ++stats->turn_speed;
          break;
        case ITEM_DAMAGE_UP:
          stats->shot_damage += 10;
          stats->reload_delay = stats->reload_delay * 105 / 100;
          break;
        case ITEM_ACCURACY_UP:
          stats->shot_spread -= 4;
          break;
        case ITEM_SHOT_SPEED_UP:
          stats->shot_speed += fixed_new(0, 128);
          stats->shot_damage = stats->shot_damage * 105 / 100;
          stats->shot_kb += fixed_new(0, 16);
          break;
        case ITEM_SHOT_COUNT_UP:
          stats->shot_count += fixed_new(0, 64);
          stats->reload_delay = stats->reload_delay * 105 / 100;
          stats->shot_spread += 2;
          break;
        case ITEM_SHOT_KB_UP:
          stats->shot_kb += fixed_new(0, 64);
          break;
        case ITEM_PIERCE_UP:
          stats->shot_pierce += 1;
          if (stats->shot_pierce < 5) {
            stats->reload_delay = stats->reload_delay * 115 / 100;
          }
          break;
        case ITEM_MAGNETISM_UP:
          stats->magnetism_dist += 16;
          stats->magnetism_percent += 20;
          break;
        case ITEM_HOMING_POWER:
          if (x == 0) {
            stats->shot_homing_power += 20;
            stats->shot_homing_percent += 20;
          } else {
            stats->shot_homing_power += 4;
            stats->shot_homing_percent += 4;
          }
          break;
        case ITEM_SIGHT_UP:
          stats->sight_range += 8;
          stats->view_distance += 8;
          break;
        case ITEM_MAX_HP_UP:
          stats->max_hp += 100;
          stats->max_hp = stats->max_hp * 11 / 10;
          stats->size += 1;
          break;
        case ITEM_REGEN_UP:
          stats->active_regen_delay = stats->active_regen_delay * 75 / 100;
          stats->active_regen = (stats->active_regen + 10) * 11 / 10;
          stats->passive_regen += 2;
          break;
        case ITEM_CONTACT_DAMAGE_UP:
          stats->contact_damage = (stats->contact_damage + 10) * 11 / 10;
          break;
        case ITEM_CREATIVITY_UP:
          stats->creativity += 12;
          break;
        case ITEM_SPLIT_SHOT:
          if (x == 0) {
            stats->shot_split_percent += 20;
          } else {
            stats->shot_split_percent += 5;
            stats->shot_split_fragments += 1;
          }
          break;
        default:
          TraceLog(LOG_WARNING, "Unhandled item with id %d", i);
      }
    }
  }

  clamp(&stats->max_hp, 10, 10000);
  clamp(&stats->active_regen_delay, 0, 1000);
  clamp(&stats->shot_damage, 10, 1000);
  fixed_clamp(&stats->max_move_speed, fixed_new(1, 0), fixed_new(4, 0));
  fixed_clamp(&stats->reload_delay, fixed_new(0, 8), fixed_new(5, 0));
  fixed_clamp(&stats->shot_count, fixed_new(1, 0), fixed_new(8, 0));
  fixed_clamp(&stats->shot_kb, fixed_new(0, 0), fixed_new(4, 0));
  clamp(&stats->shot_pierce, 1, 8);
  fixed_clamp(&stats->shot_speed, fixed_new(0, 128), fixed_new(8, 0));
  clamp(&stats->shot_spread, 0, 32);
  clamp(&stats->size, 10, 50);
  clamp(&stats->turn_speed, 1, 64);
  clamp(&stats->magnetism_dist, 16, 1000);
  clamp(&stats->magnetism_percent, 0, 100);
  clamp(&stats->sight_range, 120, 240);
  clamp(&stats->view_distance, 120, 240);
  clamp(&stats->shot_homing_percent, 0, 100);
  clamp(&stats->shot_homing_power, 0, 100);
  clamp(&stats->active_regen, 0, 1000);
  clamp(&stats->passive_regen, 0, 1000);
  clamp(&stats->creativity, 0, 100);
  clamp(&stats->shot_split_fragments, 2, 12);
  clamp(&stats->shot_split_percent, 0, 100);
}

int GsGetTextFxSlot(GameScene* GS) {
  int oldest_idx = 0;
  for (int t = 0; t < LENGTHOF(GS->text_fx); ++t) {
    if (!GS->text_fx[t].exists) {
      GS->text_fx[t].exists = true;
      return t;
    }
    if (GS->text_fx[t].despawn_timer < GS->text_fx[oldest_idx].despawn_timer) {
      oldest_idx = t;
    }
  }

  // force-override oldest text effect if there were no slots available
  GS->text_fx[oldest_idx] = (GsTextFx){0};
  GS->text_fx[oldest_idx].exists = true;
  return oldest_idx;
}

void GsSpawnXpOrb(GameScene* GS, fixed_t x, fixed_t y, int xp) {
  int oldest_idx = 0;
  for (int o = 0; o < LENGTHOF(GS->xp_orbs); ++o) {
    if (GS->xp_orbs[o].exists) {
      if (GS->xp_orbs[o].age > GS->xp_orbs[oldest_idx].age) {
        oldest_idx = o;
      }
      continue;
    }

    GS->xp_orbs[o].exists = true;
    GS->xp_orbs[o].x = x;
    GS->xp_orbs[o].y = y;
    GS->xp_orbs[o].xp = xp;
    if (GetRandomValue(0, 99) < GS->player.stats.magnetism_percent) {
      GS->xp_orbs[o].noticed_player = true;
    }
    return;
  }

  // override oldest XP orb if all slots are full
  GS->xp_orbs[oldest_idx].exists = true;
  GS->xp_orbs[oldest_idx].x = x;
  GS->xp_orbs[oldest_idx].y = y;
  GS->xp_orbs[oldest_idx].xp = xp;
  if (GetRandomValue(0, 99) < GS->player.stats.magnetism_percent) {
    GS->xp_orbs[oldest_idx].noticed_player = true;
  }
}

void GsUpdateShapes(GameScene* GS) {
  // shape logic
  for (int s = 0; s < LENGTHOF(GS->shapes); ++s) {
    if (!GS->shapes[s].exists) {
      continue;
    }

    // hp and damage-related
    if (GS->shapes[s].hp <= 0) {
      GS->shapes[s].marked_for_despawn = true;
    }
    ++GS->shapes[s].ticks_since_damaged;
    if (GS->shapes[s].i_frames > 0) {
      --GS->shapes[s].i_frames;
    }
    if (GS->ticks % 60 < GS->shapes[s].regen && (GS->shapes[s].hp < GS->shapes[s].max_hp)) {
      GS->shapes[s].hp += 10;
    }
    if (GS->shapes[s].variant == SHAPE_VARIANT_HEALING && (GS->shapes[s].hp < GS->shapes[s].max_hp)) {
      GS->shapes[s].hp += GS->shapes[s].max_hp / 240;
    }

    // despawning
    if (GS->shapes[s].marked_for_despawn) {
      // printf("Despawned shape %d\n", i);
      if (GS->shapes[s].grant_xp_on_despawn) {
        GsSpawnXpOrb(GS, GS->shapes[s].x, GS->shapes[s].y, GS->shapes[s].xp);
      }
      if (GS->shapes[s].sides > 3) {
        if (GS->shapes[s].spawn_children_on_despawn) {
          SpawnChildShapes(GS, s);
        }
      }
      GS->shapes[s] = (GsShape){0};
      GS->shapes[s].exists = false;
      --GS->shape_count;
      continue;
    }

    // movement
    fixed_t real_move_speed = GS->shapes[s].move_speed;
    if (GS->shapes[s].sqdist_to_target < int_sq(64) && GS->shapes[s].move_speed > GS->player.stats.max_move_speed) {
      real_move_speed = GS->player.stats.max_move_speed;
    }
    GS->shapes[s].x += fixed_cos(GS->shapes[s].move_angle) * real_move_speed / fixed_factor;
    GS->shapes[s].y += fixed_sin(GS->shapes[s].move_angle) * real_move_speed / fixed_factor;

    // kb
    GS->shapes[s].x += fixed_cos(GS->shapes[s].kb_angle) * GS->shapes[s].kb_speed / fixed_factor;
    GS->shapes[s].y += fixed_sin(GS->shapes[s].kb_angle) * GS->shapes[s].kb_speed / fixed_factor;
    GS->shapes[s].kb_speed -= fixed_new(0, 4);
    fixed_clamp(&GS->shapes[s].kb_speed, 0, fixed_new(64, 0));

    // update sqdist_to_player, angle_to_player
    {
      int dx = fixed_whole(GS->player.x) - fixed_whole(GS->shapes[s].x);
      int dy = fixed_whole(GS->player.y) - fixed_whole(GS->shapes[s].y);
      GS->shapes[s].sqdist_to_player = int_sq(dx) + int_sq(dy);
      GS->shapes[s].angle_to_player = angle_from_slope(dx, dy);
    }

    // targeting
    int aggro_time = 180;
    if (GS->shapes[s].variant == SHAPE_VARIANT_FAST) {
      aggro_time = 600;
    }
    if (GS->shapes[s].always_target || GS->shapes[s].ticks_since_damaged < aggro_time ||
        GS->shapes[s].sqdist_to_player < int_sq(80)) {
      // update target_x, target_y
      GS->shapes[s].target_x = GS->player.x;
      GS->shapes[s].target_y = GS->player.y;

      // update sqdist_to_target, angle_to_target
      {
        int dx = fixed_whole(GS->shapes[s].target_x) - fixed_whole(GS->shapes[s].x);
        int dy = fixed_whole(GS->shapes[s].target_y) - fixed_whole(GS->shapes[s].y);
        GS->shapes[s].sqdist_to_target = int_sq(dx) + int_sq(dy);
        GS->shapes[s].angle_to_target = angle_from_slope(dx, dy);
      }

      // FAST variants have a slower turn speed
      if (GS->shapes[s].variant == SHAPE_VARIANT_FAST) {
        angle_rotate_towards(&GS->shapes[s].move_angle, GS->shapes[s].angle_to_target, 2);
      } else {
        angle_rotate_towards(&GS->shapes[s].move_angle, GS->shapes[s].angle_to_target, 3);
      }

      // shapes use a slower max move speed when not facing target
      if (abs(angle_diff(GS->shapes[s].move_angle, GS->shapes[s].angle_to_target)) > 15) {
        fixed_nudge(&GS->shapes[s].move_speed, GS->shapes[s].max_move_speed / 2, fixed_new(0, 16));
      } else {
        fixed_nudge(&GS->shapes[s].move_speed, GS->shapes[s].max_move_speed, fixed_new(0, 8));
      }
    } else {
      fixed_nudge(&GS->shapes[s].move_speed, GS->shapes[s].max_move_speed / 2, fixed_new(0, 8));
    }

    // player distance-related despawning
    if (GS->shapes[s].sqdist_to_player > int_sq(render_w)) {
      GS->shapes[s].marked_for_despawn = true;
      GS->shapes[s].spawn_children_on_despawn = false;
    }

    // player collision
    ++GS->shapes[s].ticks_since_damaged_player;
    if (GS->shapes[s].i_frames <= 0 &&
        GS->shapes[s].ticks_since_damaged_player >= 6 &&
        GS->shapes[s].sqdist_to_player < int_sq(GS->player.stats.size + GS->shapes[s].size - 8)) {
      // damage self
      GS->shapes[s].hp -= GS->player.stats.contact_damage;
      GS->shapes[s].ticks_since_damaged = 0;
      GS->shapes[s].ticks_since_damaged_player = 0;
      GS->shapes[s].grant_xp_on_despawn = true;
      // GS->shapes[s].x += fixed_cos(GS->shapes[s].angle_to_player) * -1;
      // GS->shapes[s].y += fixed_sin(GS->shapes[s].angle_to_player) * -1;
      // GS->shapes[s].move_speed = (GS->shapes[s].max_move_speed * -2);  // use negative move speed instead of knockback stat so it doesn't override knockback

      // damage player
      if (GS->shapes[s].hp > 0) {
        GS->player.hp -= GS->shapes[s].contact_damage;
        GS->player.ticks_since_damaged = 0;
      }

      // make text fx for shape dmg
      int t = GsGetTextFxSlot(GS);
      GS->text_fx[t].x = GS->shapes[s].x;
      GS->text_fx[t].y = GS->shapes[s].y;
      GS->text_fx[t].despawn_timer = 60;
      snprintf(GS->text_fx[t].text, LENGTHOF(GS->text_fx[t].text), "-%d", GS->player.stats.contact_damage);

      // make text fx for player dmg
      // int t = GsGetTextFxSlot(GS);
      // GS->text_fx[t].x = GS->player.x;
      // GS->text_fx[t].y = GS->player.y - GS->player.stats.size;
      // GS->text_fx[t].despawn_timer = 120;
      // snprintf(GS->text_fx[t].text, LENGTHOF(GS->text_fx[t].text), "-%d", GS->shapes[s].contact_damage);
    }

    if (GS->ticks % 4 == 0) {
      // shape-shape collision
      for (int j = s + 1; j < LENGTHOF(GS->shapes); ++j) {
        if (!GS->shapes[j].exists) {
          continue;
        }
        int dx = abs(fixed_whole(GS->shapes[s].x) - fixed_whole(GS->shapes[j].x));
        int dy = abs(fixed_whole(GS->shapes[s].y) - fixed_whole(GS->shapes[j].y));
        int sqdist = int_sq(dx) + int_sq(dy);
        if (sqdist < int_sq((GS->shapes[s].size + GS->shapes[j].size) / 2)) {
          if (GS->shapes[s].x < GS->shapes[j].x) {
            GS->shapes[s].x -= fixed_new(1, 0);
            GS->shapes[j].x += fixed_new(1, 0);
          } else {
            GS->shapes[s].x += fixed_new(1, 0);
            GS->shapes[j].x -= fixed_new(1, 0);
          }
        }
      }
    }
  }
  qsort(GS->shapes, LENGTHOF(GS->shapes), sizeof(GsShape), CompareShapes);
  // for (int s = 0; s < LENGTHOF(GS->shapes); ++s) {
  //   if (GS->shapes[s].exists) {
  //     printf("%c", GS->shapes[s].sides + '0');

  //   } else {
  //     printf("_");
  //   }
  // }
  // printf(" %d\n", GS->shape_count);
}

int GsXpForLevelUp(GameScene* GS) {
  return 6 + 6 * GS->player.level;
}

void GsUpdatePlayer(GameScene* GS) {
  // player movement
  GS->player.move_speed = GS->player.stats.max_move_speed;
  if (IsKeyDown(KEY_A)) GS->player.x -= GS->player.move_speed;
  if (IsKeyDown(KEY_D)) GS->player.x += GS->player.move_speed;
  if (IsKeyDown(KEY_S)) GS->player.y += GS->player.move_speed;
  if (IsKeyDown(KEY_W)) GS->player.y -= GS->player.move_speed;

  // find index of closest shape
  int closest_sqdist = INT_MAX;
  int closest_shape_idx = -1;
  for (int s = 0; s < LENGTHOF(GS->shapes); ++s) {
    if (GS->shapes[s].exists) {
      if (GS->shapes[s].sqdist_to_player < int_sq(GS->player.stats.sight_range) &&
          GS->shapes[s].sqdist_to_player < closest_sqdist) {
        closest_shape_idx = s;
        closest_sqdist = GS->shapes[s].sqdist_to_player;
      }
    }
  }
  // rotate player towards closest shape
  if (closest_shape_idx != -1) {
    angle_t target_angle = angle_from_line(GS->player.x, GS->player.y, GS->shapes[closest_shape_idx].x, GS->shapes[closest_shape_idx].y);
    angle_rotate_towards(&GS->player.angle, target_angle, GS->player.stats.turn_speed);
  }

  // collect pickups
  for (int p = 0; p < LENGTHOF(GS->pickups); ++p) {
    if (!GS->pickups[p].exists) {
      continue;
    }
    int dx = abs(fixed_whole(GS->pickups[p].x) - fixed_whole(GS->player.x));
    int dy = abs(fixed_whole(GS->pickups[p].y) - fixed_whole(GS->player.y));
    int sqdist = int_sq(dx) + int_sq(dy);
    if (sqdist < int_sq(GS->player.stats.size)) {
      GS->pickups[p].exists = false;
      ++GS->player.item_counts[GS->pickups[p].type];
      ++GS->player.items_collected;
      GsUpdatePlayerStats(GS, &GS->player.stats);
    }
  }

  // update dps calculation
  int next_entry = (GS->ticks + 1) % LENGTHOF(GS->player.damage_history);
  int curr_entry = (GS->ticks) % LENGTHOF(GS->player.damage_history);
  GS->player.dps -= GS->player.damage_history[next_entry];
  GS->player.damage_history[next_entry] = 0;

  // regen
  ++GS->player.ticks_since_damaged;
  if (GS->player.ticks_since_damaged >= GS->player.stats.active_regen_delay &&
      GS->player.hp < GS->player.stats.max_hp &&
      GS->ticks % 10 == 0) {
    GS->player.hp += GS->player.stats.active_regen;
  }
  if (GS->player.hp < GS->player.stats.max_hp && GS->ticks % 10 == 5) {
    GS->player.hp += GS->player.stats.passive_regen;
  }

  // misc
  ++GS->player.ticks_since_last_shot;
}

void GsSpawnNewProjs(GameScene* GS) {
  // spawn new projs
  GS->player.reload_progress += fixed_factor * fixed_factor / target_fps / GS->player.stats.reload_delay;
  while (GS->player.reload_progress >= fixed_factor) {
    GS->player.reload_progress -= fixed_factor;
    GS->player.shot_progress += GS->player.stats.shot_count;
  }
  // who knows, maybe you could shoot multiple volleys in one tick
  while (GS->player.shot_progress >= fixed_new(1, 0)) {
    GS->player.shot_progress -= fixed_new(1, 0);
    GS->player.ticks_since_last_shot = 0;

    // volley of projectiles
    bool first_shot_in_volley = true;
    bool volley_is_homing = (GetRandomValue(0, 99) < GS->player.stats.shot_homing_percent);
    bool volley_is_splitting = (GetRandomValue(0, 99) < GS->player.stats.shot_split_percent);
    if (volley_is_splitting) {
      // double reload delay after a splitting shot
      GS->player.shot_progress -= fixed_new(1, 0);
    }
    for (int p = 0; p < LENGTHOF(GS->projs); ++p) {
      if (GS->projs[p].exists) {
        continue;
      }
      GS->projs[p].exists = true;
      GS->projs[p].pierce = GS->player.stats.shot_pierce;
      GS->projs[p].x = GS->player.x;
      GS->projs[p].y = GS->player.y;

      // effects which only apply to the first shot of the volley
      if (first_shot_in_volley) {
        GS->projs[p].move_speed = GS->player.stats.shot_speed;
        GS->projs[p].angle = GS->player.angle + GetRandomValue(-GS->player.stats.shot_spread, GS->player.stats.shot_spread);
        GS->projs[p].size = 6 + (GS->player.stats.shot_damage / 40);
        GS->projs[p].damage = GS->player.stats.shot_damage;
        GS->projs[p].despawn_timer = 120;
        GS->projs[p].kb = GS->player.stats.shot_kb;
        first_shot_in_volley = false;
      } else {
        GS->projs[p].move_speed = GS->player.stats.shot_speed * GetRandomValue(50, 90) / 100;
        GS->projs[p].angle = GS->player.angle + GetRandomValue(-GS->player.stats.shot_spread * 2, GS->player.stats.shot_spread * 2);
        GS->projs[p].size = 6 + (GS->player.stats.shot_damage / 80);
        GS->projs[p].damage = GS->player.stats.shot_damage / 2;
        GS->projs[p].despawn_timer = 120;
        GS->projs[p].kb = GS->player.stats.shot_kb / 2;
      }
      // special effects that apply to the entire volley
      if (volley_is_homing) {
        GS->projs[p].homing_max_dist = 256;
        GS->projs[p].homing_power = GS->player.stats.shot_homing_power;
        GS->projs[p].despawn_timer *= 2;
        GS->projs[p].move_speed /= 2;
      }
      if (volley_is_splitting) {
        GS->projs[p].damage *= 2;
        GS->projs[p].split_fragments = GS->player.stats.shot_split_fragments;
        GS->projs[p].size = (GS->projs[p].size * 3 / 2);
      }

      GS->projs[p].x += fixed_cos(GS->projs[p].angle) * GS->player.stats.size;
      GS->projs[p].y += fixed_sin(GS->projs[p].angle) * GS->player.stats.size;
      // printf("Spawned proj %d\n", i);
      break;
    }
  }
}

void GsSpawnSplitProjs(GameScene* GS, int p) {
  int delta = angle_factor / GS->projs[p].split_fragments;
  angle_t ang = GS->projs[p].angle + delta / 2;
  for (int i = 0; i < GS->projs[p].split_fragments; ++i) {
    ang += delta;
    for (int q = 0; q < LENGTHOF(GS->projs); ++q) {
      if (GS->projs[q].exists) {
        continue;
      }
      GS->projs[q] = GS->projs[p];
      GS->projs[q].angle = ang;
      GS->projs[q].split_fragments = 0;
      GS->projs[q].size /= 2;
      GS->projs[q].despawn_timer = 60;
      GS->projs[q].pierce = 1;

      break;
    }
  }
}

void GsUpdateProjs(GameScene* GS) {
  // proj logic
  for (int p = 0; p < LENGTHOF(GS->projs); ++p) {
    if (!GS->projs[p].exists) {
      continue;
    }
    // despawn timer
    --GS->projs[p].despawn_timer;
    if (GS->projs[p].despawn_timer <= 0) {
      // printf("Despawned proj %d\n", p);
      GS->projs[p] = (GsProj){0};
      GS->projs[p].exists = false;
      continue;
    }

    // homing
    GS->projs[p].is_homing = false;
    if (GS->projs[p].homing_max_dist > 0 && GS->projs[p].homing_power > 0) {
      // find nearest shape we haven't hit yet
      int nearest_shape_idx = -1;
      int s_sqdist = INT_MAX;
      for (int s = 0; s < LENGTHOF(GS->shapes); ++s) {
        if (!GS->shapes[s].exists) {
          continue;
        }
        bool already_hit = false;
        for (int h = 0; h < LENGTHOF(GS->projs[p].hit_shape_ids); ++h) {
          if (GS->projs[p].hit_shape_ids[h] == GS->shapes[s].id) {
            already_hit = true;
            break;
          }
        }
        if (already_hit) {
          continue;
        }
        int dx = fixed_whole(GS->shapes[s].x) - fixed_whole(GS->projs[p].x);
        int dy = fixed_whole(GS->shapes[s].y) - fixed_whole(GS->projs[p].y);
        int sqdist = int_sq(dx) + int_sq(dy);
        if (sqdist < s_sqdist) {
          s_sqdist = sqdist;
          nearest_shape_idx = s;
        }
      }
      // home, then update homing power
      if (nearest_shape_idx != -1 && s_sqdist < int_sq(GS->projs[p].homing_max_dist + GS->shapes[nearest_shape_idx].size)) {
        angle_t target_angle = angle_from_line(GS->projs[p].x, GS->projs[p].y, GS->shapes[nearest_shape_idx].x, GS->shapes[nearest_shape_idx].y);
        if (target_angle != GS->projs[p].angle) {
          GS->projs[p].is_homing = true;
          angle_rotate_towards(&GS->projs[p].angle, target_angle, 8);
          --GS->projs[p].homing_power;
        }
        // make text fx
        int t = GsGetTextFxSlot(GS);
        GS->text_fx[t].x = GS->projs[p].x;
        GS->text_fx[t].y = GS->projs[p].y - 64;
        GS->text_fx[t].despawn_timer = 30;
        snprintf(GS->text_fx[t].text, LENGTHOF(GS->text_fx[t].text), ".");
      }
    }

    // movement
    GS->projs[p].x += fixed_cos(GS->projs[p].angle) * GS->projs[p].move_speed / fixed_factor;
    GS->projs[p].y += fixed_sin(GS->projs[p].angle) * GS->projs[p].move_speed / fixed_factor;

    // shape collision
    bool hit_something = false;
    for (int s = 0; s < LENGTHOF(GS->shapes); ++s) {
      if (!GS->shapes[s].exists) {
        continue;
      }
      if (GS->shapes[s].i_frames > 0) {
        continue;
      }
      int dx = abs(fixed_whole(GS->shapes[s].x) - fixed_whole(GS->projs[p].x));
      int dy = abs(fixed_whole(GS->shapes[s].y) - fixed_whole(GS->projs[p].y));
      int sqdist = int_sq(dx) + int_sq(dy);
      if (sqdist >= int_sq(GS->shapes[s].size + GS->projs[p].size)) {
        continue;
      }
      // see if shape has been hit already
      bool skip_this_shape = false;
      for (int h = 0; h < LENGTHOF(GS->projs[p].hit_shape_ids); ++h) {
        if (GS->projs[p].hit_shape_ids[h] == GS->shapes[s].id) {
          // found shape in hit-shapes list
          skip_this_shape = true;
          break;
        }
        if (GS->projs[p].hit_shape_ids[h] == 0) {
          // reached end of hit-shapes list, add new shape
          GS->projs[p].hit_shape_ids[h] = GS->shapes[s].id;
          break;
        }
      }
      if (skip_this_shape) {
        continue;
      }

      hit_something = true;

      // damage the shape
      GS->shapes[s].hp -= GS->projs[p].damage;
      GS->shapes[s].ticks_since_damaged = 0;
      GS->shapes[s].kb_angle = GS->projs[p].angle;
      GS->shapes[s].kb_speed = fixed_max(GS->shapes[s].kb_speed, GS->projs[p].kb * GS->shapes[s].max_move_speed / fixed_factor);

      // make text fx
      int t = GsGetTextFxSlot(GS);
      GS->text_fx[t].x = GS->shapes[s].x;
      GS->text_fx[t].y = GS->shapes[s].y;
      GS->text_fx[t].despawn_timer = 60;
      snprintf(GS->text_fx[t].text, LENGTHOF(GS->text_fx[t].text), "-%d", GS->projs[p].damage);

      // do player dps tracking
      GS->player.dps += GS->projs[p].damage;
      GS->player.damage_history[GS->ticks % LENGTHOF(GS->player.damage_history)] += GS->projs[p].damage;
      if (GS->shapes[s].hp <= 0) {
        GS->shapes[s].grant_xp_on_despawn = true;
      }

      // queue proj for despawn
      GS->projs[p].pierce -= 1;
      if (GS->projs[p].pierce <= 0) {
        GS->projs[p].despawn_timer = 0;
        break;
      }
    }

    if (hit_something && GS->projs[p].split_fragments > 0) {
      GsSpawnSplitProjs(GS, p);
    }
  }
}

void GsUpdatePickups(GameScene* GS) {
  for (int p = 0; p < LENGTHOF(GS->pickups); ++p) {
    if (!GS->pickups[p].exists) {
      continue;
    }

    // update sqdist_to_player
    {
      int dx = fixed_whole(GS->player.x) - fixed_whole(GS->pickups[p].x);
      int dy = fixed_whole(GS->player.y) - fixed_whole(GS->pickups[p].y);
      GS->pickups[p].sqdist_to_player = int_sq(dx) + int_sq(dy);
      GS->pickups[p].angle_to_player = angle_from_slope(dx, dy);
    }

    if (GS->pickups[p].sqdist_to_player < int_sq(GS->player.stats.magnetism_dist)) {
      GS->pickups[p].x += fixed_cos(GS->pickups[p].angle_to_player) * GS->player.stats.max_move_speed / fixed_factor;
      GS->pickups[p].y += fixed_sin(GS->pickups[p].angle_to_player) * GS->player.stats.max_move_speed / fixed_factor;
    }

    // move faraway off-screen items closer
    if (GS->pickups[p].sqdist_to_player > int_sq(render_w * 2)) {
      GS->pickups[p].x += fixed_cos(GS->pickups[p].angle_to_player) * render_w;
      GS->pickups[p].y += fixed_sin(GS->pickups[p].angle_to_player) * render_w;
    }
  }
}

void GsUpdateTextFx(GameScene* GS) {
  for (int t = 0; t < LENGTHOF(GS->text_fx); ++t) {
    if (!GS->text_fx[t].exists) {
      continue;
    }
    --GS->text_fx[t].despawn_timer;
    GS->text_fx[t].y -= fixed_new(0, 128);
    if (GS->text_fx[t].despawn_timer <= 0) {
      GS->text_fx[t].exists = false;
    }
  }
}

void GsLevelUpPlayer(GameScene* GS) {
  GS->curr_overlay = GS_OVERLAY_PICK_ITEM;
  GS->ol_pick_item = (GsOlPickItem){0};
  GS->ol_pick_item.item_count = 3;
  for (int c = 0; c < GS->ol_pick_item.item_count; ++c) {
    GsOlPickItemChoice* choice = &GS->ol_pick_item.choices[c];

  reroll:
    *choice = (GsOlPickItemChoice){
        .item_a = GetRandomValue(0, ITEM_COUNT - 1),
        .item_b = ITEM_INVALID,
        .removed_item = ITEM_INVALID};

    // reroll item_a until it's not equal to any other item_a
    for (int c2 = 0; c2 < GS->ol_pick_item.item_count; ++c2) {
      if (c != c2 && choice->item_a == GS->ol_pick_item.choices[c2].item_a) {
        goto reroll;
      }
    }

    // Creative choices
    if (GetRandomValue(0, 99) < GS->player.stats.creativity) {
      int quality = GetRandomValue(0, 2);
      // traded item
      if (quality == 0 || quality == 1) {
        int traded = GetRandomValue(0, ITEM_COUNT - 1);
        if (traded != choice->item_a && GS->player.item_counts[traded] > 0) {
          choice->removed_item = traded;
        }
      }
      // bonus item
      if (quality == 1 || quality == 2) {
        choice->item_b = GetRandomValue(0, ITEM_COUNT - 1);
      }
    }
  }
  if (GS->player.level > 80) {
    GS->curr_overlay = GS_OVERLAY_NONE;
  }

  GS->player.xp -= GsXpForLevelUp(GS);
  // GsSpawnPickup(GS, GS->player.x, GS->player.y - fixed_new(60, 0));
  ++GS->player.level;
}

void GsUpdateXpOrbs(GameScene* GS) {
  for (int o = 0; o < LENGTHOF(GS->xp_orbs); ++o) {
    if (!GS->xp_orbs[o].exists) {
      continue;
    }

    GS->xp_orbs[o].x += fixed_cos(GS->xp_orbs[o].angle) * GS->xp_orbs[o].move_speed / fixed_factor;
    GS->xp_orbs[o].y += fixed_sin(GS->xp_orbs[o].angle) * GS->xp_orbs[o].move_speed / fixed_factor;
    ++GS->xp_orbs[o].age;

    int dx = fixed_whole(GS->player.x) - fixed_whole(GS->xp_orbs[o].x);
    int dy = fixed_whole(GS->player.y) - fixed_whole(GS->xp_orbs[o].y);
    angle_t target_angle = angle_from_slope(dx, dy);
    angle_rotate_towards(&GS->xp_orbs[o].angle, target_angle, 4);

    int sqdist_to_player = int_sq(dx) + int_sq(dy);
    if (sqdist_to_player < int_sq(GS->player.stats.magnetism_dist) + int_sq(GS->player.stats.size)) {
      GS->xp_orbs[o].noticed_player = true;
    }

    if (GS->xp_orbs[o].noticed_player) {
      if (angle_diff(GS->xp_orbs[o].angle, target_angle) > 15) {
        fixed_nudge(&GS->xp_orbs[o].move_speed, 0, fixed_new(0, 16));
      } else {
        fixed_nudge(&GS->xp_orbs[o].move_speed, fixed_new(256, 0) / target_fps, fixed_new(0, 16));
      }

      if (sqdist_to_player < int_sq(GS->player.stats.size)) {
        GS->player.total_xp += GS->xp_orbs[o].xp;
        GS->player.xp += GS->xp_orbs[o].xp;
        if (GS->player.xp >= GsXpForLevelUp(GS)) {
          GsLevelUpPlayer(GS);
        }
        for (int o2 = 0; o2 < LENGTHOF(GS->xp_orbs); ++o2) {
          if (!GS->xp_orbs[o2].exists) {
            continue;
          }
          GS->xp_orbs[o2].noticed_player = true;
        }

        GS->xp_orbs[o] = (GsXpOrb){0};
        GS->xp_orbs[o].exists = false;
      }
    }
  }
}

void GsUpdateCamera(GameScene* GS) {
  GS->camera.x = GS->player.x - GetRenderLength(GS, render_w / 2, default_z);
  GS->camera.y = GS->player.y - GetRenderLength(GS, render_h / 2, default_z);

  fixed_t target_zoom = 256 * 256 / (GS->player.stats.view_distance * fixed_factor * 2 / render_h);
  fixed_nudge(&GS->camera.zoom, target_zoom, 1);
}

void GsUpdateOlPickItem(GameScene* GS) {
  if (GS->ol_pick_item.item_count <= 0) {
    GS->curr_overlay = GS_OVERLAY_NONE;
    TraceLog(LOG_ERROR, "No items in pick item overlay");
    return;
  }

  if (IsKeyPressed(KEY_S)) {
    GS->ol_pick_item.selected_item_idx += 1;
  };
  if (IsKeyPressed(KEY_W)) {
    GS->ol_pick_item.selected_item_idx += (GS->ol_pick_item.item_count - 1);
  };
  GS->ol_pick_item.selected_item_idx %= GS->ol_pick_item.item_count;

  GsOlPickItemChoice choice = GS->ol_pick_item.choices[GS->ol_pick_item.selected_item_idx];

  ++GS->player.item_counts[choice.item_a];
  if (choice.item_b != ITEM_INVALID) ++GS->player.item_counts[choice.item_b];
  if (choice.removed_item != ITEM_INVALID) --GS->player.item_counts[choice.removed_item];
  GsUpdatePlayerStats(GS, &GS->player.tmp_stats);
  --GS->player.item_counts[choice.item_a];
  if (choice.item_b != ITEM_INVALID) --GS->player.item_counts[choice.item_b];
  if (choice.removed_item != ITEM_INVALID) ++GS->player.item_counts[choice.removed_item];

  if (IsKeyPressed(KEY_SPACE)) {
    ++GS->player.item_counts[choice.item_a];
    if (choice.item_b != ITEM_INVALID) ++GS->player.item_counts[choice.item_b];
    if (choice.removed_item != ITEM_INVALID) --GS->player.item_counts[choice.removed_item];
    GS->player.stats = GS->player.tmp_stats;
    GS->curr_overlay = GS_OVERLAY_NONE;
  }
}

void GsInit(GameScene* GS) {
  GsUpdatePlayerStats(GS, &GS->player.stats);
  GS->player.hp = GS->player.stats.max_hp;
  GS->camera.zoom = fixed_new(1, 0);
}

void GsUpdate(GameScene* GS) {
  switch (GS->curr_overlay) {
    case GS_OVERLAY_NONE: {
      GsSpawnNewShapes(GS);
      GsUpdateShapes(GS);

      GsUpdatePlayer(GS);

      GsSpawnNewProjs(GS);
      GsUpdateProjs(GS);

      GsUpdatePickups(GS);
      GsUpdateTextFx(GS);
      GsUpdateXpOrbs(GS);
      GsUpdateCamera(GS);
      ++GS->ticks;
    } break;

    case GS_OVERLAY_PICK_ITEM: {
      GsUpdateOlPickItem(GS);
      ++GS->ol_pick_item.ticks;
    } break;

    default: {
      TraceLog(LOG_ERROR, "Bad overlay value %d", GS->curr_overlay);
    } break;
  }
}
