#include "gs_update.h"

#include <stdlib.h>
#include <string.h>

#include "camera_util.h"
#include "shape_util.h"

void GsSpawnNewShapes(GameScene* GS) {
  int shape_limit = 25 + GS->player.level;
  if (GS->ticks % 2 == 0 && GS->shape_count < (LENGTHOF(GS->shapes) / 2) && GS->shape_count < shape_limit) {
    fixed_t new_x = GS->player.x + FixNew(GetRandomValue(-render_w, render_w), 0);
    fixed_t new_y = GS->player.y + FixNew(GetRandomValue(-render_h, render_h), 0);
    if (FixAbs(new_x - GS->player.x) / fixed_factor < render_w / 2 &&
        FixAbs(new_y - GS->player.y) / fixed_factor < render_h / 2) {
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
  stats->max_move_speed = FixNew(1, 128);
  stats->reload_delay = FixNew(0, 80);
  stats->shot_count = FixNew(1, 0);
  stats->shot_kb = FixNew(0, 0);
  stats->shot_pierce = 1;
  stats->shot_speed = FixNew(2, 0);
  stats->shot_spread = 1;
  stats->sight_range = render_h / 2;
  stats->size = 12;
  stats->turn_speed = 2;
  stats->magnetism_dist = 64;
  stats->magnetism_frequency = INT_MAX;
  stats->shot_homing_percent = 0;
  stats->shot_homing_power = 0;
  stats->view_distance = 120;
  stats->contact_damage = 20;
  stats->active_regen = 20;
  stats->passive_regen = 0;
  stats->creativity = 0;
  stats->shot_split_fragments = 2;
  stats->shot_split_percent = 0;
  stats->shot_frost_percent = 0;
  stats->max_orbitals = 0;
  for (int i = 0; i < ITEM_COUNT; ++i) {
    for (int x = 0; x < GS->player.item_counts[i]; ++x) {
      switch (i) {
        case ITEM_SPEED_UP:
          stats->max_move_speed += FixNew(0, 96);
          break;
        case ITEM_FIRE_RATE_UP:
          stats->reload_delay = stats->reload_delay * 80 / 100;
          stats->shot_spread += 3;
          break;
        case ITEM_TURN_SPEED_UP:
          ++stats->turn_speed;
          stats->shot_spread -= 4;
          break;
        case ITEM_DAMAGE_UP:
          stats->shot_damage += 10;
          stats->reload_delay = stats->reload_delay * 105 / 100;
          break;
        // case ITEM_ACCURACY_UP:
        //   stats->shot_spread -= 4;
        //   break;
        case ITEM_SHOT_SPEED_UP:
          stats->shot_speed += FixNew(0, 128);
          stats->shot_damage = stats->shot_damage * 105 / 100;
          stats->shot_kb += FixNew(0, 16);
          break;
        case ITEM_SHOT_COUNT_UP:
          stats->shot_count += FixNew(0, 64);
          stats->reload_delay = stats->reload_delay * 105 / 100;
          stats->shot_spread += 2;
          break;
        case ITEM_SHOT_KB_UP:
          stats->shot_kb += FixNew(0, 64);
          break;
        case ITEM_PIERCE_UP:
          stats->shot_pierce += 1;
          if (stats->shot_pierce < 5) {
            stats->reload_delay = stats->reload_delay * 130 / 100;
          }
          break;
        case ITEM_MAGNETISM_UP:
          stats->magnetism_dist += 16;
          if (stats->magnetism_frequency == INT_MAX) {
            stats->magnetism_frequency = 300;
          } else {
            stats->magnetism_frequency = stats->magnetism_frequency * 4 / 5;
          }
          break;
        case ITEM_HOMING_POWER:
          if (x == 0) {
            stats->shot_homing_percent += 16;
            stats->shot_homing_power += 20;
          } else {
            stats->shot_homing_percent += 8;
            stats->shot_homing_power += 12;
          }
          break;
        case ITEM_SIGHT_UP:
          stats->sight_range += 12;
          stats->view_distance += 12;
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
          if (x == 0) {
            stats->creativity = 12;
          } else {
            stats->creativity += 6;
          }
          break;
        case ITEM_SPLIT_SHOT:
          if (x == 0) {
            stats->shot_split_percent += 20;
          } else {
            stats->shot_split_percent += 5;
            stats->shot_split_fragments += 1;
          }
          break;
        case ITEM_FROST_SHOT:
          if (x == 0) {
            stats->shot_frost_percent = 8;
          } else {
            stats->shot_frost_percent += 4;
          }
          break;
        case ITEM_ORBITALS_UP:
          stats->max_orbitals += 2;
          break;
        default:
          TraceLog(LOG_WARNING, "Unhandled item with id %d", i);
      }
    }
  }

  IntClamp(&stats->max_hp, 10, 10000);
  IntClamp(&stats->active_regen_delay, 0, 1000);
  IntClamp(&stats->shot_damage, 10, 1000);
  FixClamp(&stats->max_move_speed, FixNew(1, 0), FixNew(4, 0));
  FixClamp(&stats->reload_delay, FixNew(0, 8), FixNew(5, 0));
  FixClamp(&stats->shot_count, FixNew(1, 0), FixNew(8, 0));
  FixClamp(&stats->shot_kb, FixNew(0, 0), FixNew(4, 0));
  IntClamp(&stats->shot_pierce, 1, 8);
  FixClamp(&stats->shot_speed, FixNew(0, 128), FixNew(8, 0));
  IntClamp(&stats->shot_spread, 0, 32);
  IntClamp(&stats->size, 10, 50);
  IntClamp(&stats->turn_speed, 1, 64);
  IntClamp(&stats->magnetism_dist, 0, stats->sight_range);
  IntClamp(&stats->magnetism_frequency, 15, INT_MAX);
  IntClamp(&stats->sight_range, 120, 240);
  IntClamp(&stats->view_distance, 120, 240);
  IntClamp(&stats->shot_homing_percent, 0, 100);
  IntClamp(&stats->shot_homing_power, 0, 100);
  IntClamp(&stats->active_regen, 0, 1000);
  IntClamp(&stats->passive_regen, 0, 1000);
  IntClamp(&stats->creativity, 0, 100);
  IntClamp(&stats->shot_split_fragments, 2, 12);
  IntClamp(&stats->shot_split_percent, 0, 100);
  IntClamp(&stats->shot_frost_percent, 0, 50);
  IntClamp(&stats->max_orbitals, 0, LENGTHOF(GS->projs) / 2);
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
  int furthest_idx = 0;
  int furthest_sqdist = 0;
  for (int o = 0; o < LENGTHOF(GS->xp_orbs); ++o) {
    if (GS->xp_orbs[o].exists) {
      // get square distance of orb to player
      int sqdist = IntSq(GS->xp_orbs[o].x - GS->player.x) + IntSq(GS->xp_orbs[o].y - GS->player.y);

      if (sqdist > furthest_sqdist) {
        furthest_idx = o;
        furthest_sqdist = sqdist;
      }
      continue;
    }

    GS->xp_orbs[o] = (GsXpOrb){0};
    GS->xp_orbs[o].exists = true;
    GS->xp_orbs[o].x = x;
    GS->xp_orbs[o].y = y;
    GS->xp_orbs[o].xp = xp;
    return;
  }

  // override farthest XP orb if all slots are full
  GS->xp_orbs[furthest_idx] = (GsXpOrb){0};
  GS->xp_orbs[furthest_idx].exists = true;
  GS->xp_orbs[furthest_idx].x = x;
  GS->xp_orbs[furthest_idx].y = y;
  GS->xp_orbs[furthest_idx].xp = xp;
}

void GsUpdateShapes(GameScene* GS) {
  // shape logic
  for (int s = 0; s < LENGTHOF(GS->shapes); ++s) {
    if (!GS->shapes[s].exists) {
      continue;
    }

    // magnet pulse
    if (GS->player.ticks_since_magnetism == 0 && GS->shapes[s].sqdist_to_player < IntSq(GS->player.stats.magnetism_dist)) {
      GS->shapes[s].move_speed = 0;
    }

    // hp and damage-related
    if (GS->shapes[s].hp <= 0) {
      GS->shapes[s].marked_for_despawn = true;
    }
    ++GS->shapes[s].ticks_since_damaged;
    if (GS->shapes[s].i_frames > 0) {
      --GS->shapes[s].i_frames;
    }
    if (GS->shapes[s].frost_ticks <= 0) {
      if (GS->ticks % 60 < GS->shapes[s].regen && (GS->shapes[s].hp < GS->shapes[s].max_hp)) {
        GS->shapes[s].hp += 10;
      }
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

    // frost
    if (GS->shapes[s].frost_ticks > 0) {
      --GS->shapes[s].frost_ticks;
      GS->shapes[s].move_speed = 0;
    }
    if (GS->shapes[s].frost_ticks == 1) {
      // damage self
      GS->shapes[s].hp -= 200;
      GS->shapes[s].ticks_since_damaged = 0;
      GS->shapes[s].grant_xp_on_despawn = true;
      // make text fx for shape dmg
      int t = GsGetTextFxSlot(GS);
      GS->text_fx[t].x = GS->shapes[s].x;
      GS->text_fx[t].y = GS->shapes[s].y;
      GS->text_fx[t].despawn_timer = 60;
      snprintf(GS->text_fx[t].text, LENGTHOF(GS->text_fx[t].text), "-%d", 200);
    }

    // movement
    fixed_t real_move_speed = GS->shapes[s].move_speed;
    if (GS->shapes[s].sqdist_to_target < IntSq(64) && GS->shapes[s].move_speed > GS->player.stats.max_move_speed) {
      real_move_speed = GS->player.stats.max_move_speed;
    }
    FixMove(&GS->shapes[s].x, &GS->shapes[s].y, real_move_speed, GS->shapes[s].move_angle);

    // kb
    FixMove(&GS->shapes[s].x, &GS->shapes[s].y, GS->shapes[s].kb_speed, GS->shapes[s].kb_angle);
    GS->shapes[s].kb_speed -= FixNew(0, 4);
    if (GS->shapes[s].sqdist_to_player > IntSq(render_w / 2)) {
      GS->shapes[s].kb_speed = GS->shapes[s].kb_speed * 7 / 8;
    }
    FixClamp(&GS->shapes[s].kb_speed, 0, FixNew(64, 0));

    // update sqdist_to_player, angle_to_player
    {
      int dx = FixWhole(GS->player.x) - FixWhole(GS->shapes[s].x);
      int dy = FixWhole(GS->player.y) - FixWhole(GS->shapes[s].y);
      GS->shapes[s].sqdist_to_player = IntSq(dx) + IntSq(dy);
      GS->shapes[s].angle_to_player = AngFromSlope(dx, dy);
    }

    // targeting
    int aggro_time = 180;
    if (GS->shapes[s].variant == SHAPE_VARIANT_FAST) {
      aggro_time = 600;
    }
    if (GS->shapes[s].always_target || GS->shapes[s].ticks_since_damaged < aggro_time ||
        GS->shapes[s].sqdist_to_player < IntSq(80)) {
      // update target_x, target_y
      GS->shapes[s].target_x = GS->player.x;
      GS->shapes[s].target_y = GS->player.y;

      // update sqdist_to_target, angle_to_target
      {
        int dx = FixWhole(GS->shapes[s].target_x) - FixWhole(GS->shapes[s].x);
        int dy = FixWhole(GS->shapes[s].target_y) - FixWhole(GS->shapes[s].y);
        GS->shapes[s].sqdist_to_target = IntSq(dx) + IntSq(dy);
        GS->shapes[s].angle_to_target = AngFromSlope(dx, dy);
      }

      // FAST variants have a slower turn speed
      if (GS->shapes[s].variant == SHAPE_VARIANT_FAST) {
        AngRotateTowards(&GS->shapes[s].move_angle, GS->shapes[s].angle_to_target, 2);
      } else {
        AngRotateTowards(&GS->shapes[s].move_angle, GS->shapes[s].angle_to_target, 3);
      }

      // shapes use a slower max move speed when not facing target
      if (abs(AngDiff(GS->shapes[s].move_angle, GS->shapes[s].angle_to_target)) > 15) {
        FixNudge(&GS->shapes[s].move_speed, GS->shapes[s].max_move_speed / 2, FixNew(0, 16));
      } else {
        FixNudge(&GS->shapes[s].move_speed, GS->shapes[s].max_move_speed, FixNew(0, 8));
      }
    } else {
      FixNudge(&GS->shapes[s].move_speed, GS->shapes[s].max_move_speed / 2, FixNew(0, 8));
    }

    // player distance-related despawning
    if (GS->shapes[s].sqdist_to_player > IntSq(render_w * 2)) {
      GS->shapes[s].marked_for_despawn = true;
      GS->shapes[s].spawn_children_on_despawn = false;
    }

    // player collision
    ++GS->shapes[s].ticks_since_damaged_player;
    if (GS->shapes[s].i_frames <= 0 &&
        GS->shapes[s].ticks_since_damaged_player >= 6 &&
        GS->shapes[s].sqdist_to_player < IntSq(GS->player.stats.size + GS->shapes[s].size - 8)) {
      // damage self
      GS->shapes[s].hp -= GS->player.stats.contact_damage;
      GS->shapes[s].ticks_since_damaged = 0;
      GS->shapes[s].ticks_since_damaged_player = 0;
      GS->shapes[s].grant_xp_on_despawn = true;
      // GS->shapes[s].x += FixCos(GS->shapes[s].angle_to_player) * -1;
      // GS->shapes[s].y += FixSin(GS->shapes[s].angle_to_player) * -1;
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
        int dx = abs(FixWhole(GS->shapes[s].x) - FixWhole(GS->shapes[j].x));
        int dy = abs(FixWhole(GS->shapes[s].y) - FixWhole(GS->shapes[j].y));
        int sqdist = IntSq(dx) + IntSq(dy);
        if (sqdist < IntSq((GS->shapes[s].size + GS->shapes[j].size) / 2)) {
          if (GS->shapes[s].x < GS->shapes[j].x) {
            GS->shapes[s].x -= FixNew(1, 0);
            GS->shapes[j].x += FixNew(1, 0);
          } else {
            GS->shapes[s].x += FixNew(1, 0);
            GS->shapes[j].x -= FixNew(1, 0);
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
  return 6 + 4 * GS->player.level;
}

void GsOfferUpgrade(GameScene* GS) {
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
}

void GsUpdatePlayer(GameScene* GS) {
  // player movement
  GS->player.move_speed = GS->player.stats.max_move_speed;
  if (IsKeyDown(KEY_A)) GS->player.x -= GS->player.move_speed;
  if (IsKeyDown(KEY_D)) GS->player.x += GS->player.move_speed;
  if (IsKeyDown(KEY_S)) GS->player.y += GS->player.move_speed;
  if (IsKeyDown(KEY_W)) GS->player.y -= GS->player.move_speed;

  if (IsKeyPressed(KEY_SPACE) && GS->player.upgrades_pending > 0) {
    GsOfferUpgrade(GS);
  }

  // find index of closest shape
  int closest_sqdist = INT_MAX;
  int closest_shape_idx = -1;
  for (int s = 0; s < LENGTHOF(GS->shapes); ++s) {
    if (GS->shapes[s].exists) {
      if (GS->shapes[s].sqdist_to_player < IntSq(GS->player.stats.sight_range) &&
          GS->shapes[s].sqdist_to_player < closest_sqdist) {
        closest_shape_idx = s;
        closest_sqdist = GS->shapes[s].sqdist_to_player;
      }
    }
  }
  // rotate player towards closest shape
  if (closest_shape_idx != -1) {
    angle_t target_angle = AngFromLine(GS->player.x, GS->player.y, GS->shapes[closest_shape_idx].x, GS->shapes[closest_shape_idx].y);
    AngRotateTowards(&GS->player.angle, target_angle, GS->player.stats.turn_speed);
  }

  // collect pickups
  for (int p = 0; p < LENGTHOF(GS->pickups); ++p) {
    if (!GS->pickups[p].exists) {
      continue;
    }
    int dx = abs(FixWhole(GS->pickups[p].x) - FixWhole(GS->player.x));
    int dy = abs(FixWhole(GS->pickups[p].y) - FixWhole(GS->player.y));
    int sqdist = IntSq(dx) + IntSq(dy);
    if (sqdist < IntSq(GS->player.stats.size)) {
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
  ++GS->player.ticks_since_magnetism;
  if (GS->player.ticks_since_magnetism >= GS->player.stats.magnetism_frequency) {
    GS->player.ticks_since_magnetism = 0;
  }
}

void GsSpawnNewProjs(GameScene* GS) {
  // spawn new projs
  GS->player.reload_progress += fixed_factor * fixed_factor / target_fps / GS->player.stats.reload_delay;
  while (GS->player.reload_progress >= fixed_factor) {
    GS->player.reload_progress -= fixed_factor;
    GS->player.shot_progress += GS->player.stats.shot_count;
  }
  // who knows, maybe you could shoot multiple volleys in one tick
  while (GS->player.shot_progress >= FixNew(1, 0)) {
    GS->player.shot_progress -= FixNew(1, 0);
    GS->player.ticks_since_last_shot = 0;

    // volley of projectiles
    bool first_shot_in_volley = true;
    bool volley_is_homing = (GetRandomValue(0, 99) < GS->player.stats.shot_homing_percent);
    bool volley_is_splitting = (GetRandomValue(0, 99) < GS->player.stats.shot_split_percent);
    bool volley_is_frost = (GetRandomValue(0, 99) < GS->player.stats.shot_frost_percent);
    bool volley_is_orbit = (GS->orbital_proj_count < GS->player.stats.max_orbitals);
    if (volley_is_splitting) {
      // double reload delay after a splitting shot
      GS->player.shot_progress -= FixNew(1, 0);
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
        GS->projs[p].move_angle = GS->player.angle + GetRandomValue(-GS->player.stats.shot_spread, GS->player.stats.shot_spread);
        GS->projs[p].size = 6 + (GS->player.stats.shot_damage / 40);
        GS->projs[p].damage = GS->player.stats.shot_damage;
        GS->projs[p].despawn_timer = 120;
        GS->projs[p].kb = GS->player.stats.shot_kb;
        first_shot_in_volley = false;
      } else {
        GS->projs[p].move_speed = GS->player.stats.shot_speed * GetRandomValue(50, 90) / 100;
        GS->projs[p].move_angle = GS->player.angle + GetRandomValue(-GS->player.stats.shot_spread * 2, GS->player.stats.shot_spread * 2);
        GS->projs[p].size = 6 + (GS->player.stats.shot_damage / 80);
        GS->projs[p].damage = GS->player.stats.shot_damage / 2;
        GS->projs[p].despawn_timer = 120;
        GS->projs[p].kb = GS->player.stats.shot_kb / 2;
      }
      // special effects that apply to the entire volley
      if (volley_is_homing) {
        GS->projs[p].homing_max_dist = 120;
        GS->projs[p].homing_power = GS->player.stats.shot_homing_power;
        GS->projs[p].despawn_timer *= 2;
        GS->projs[p].move_speed = GS->projs[p].move_speed * 2 / 3;
      }
      if (volley_is_splitting) {
        GS->projs[p].damage *= 2;
        GS->projs[p].split_fragments = GS->player.stats.shot_split_fragments;
        GS->projs[p].size = (GS->projs[p].size * 3 / 2);
      }
      if (volley_is_frost) {
        GS->projs[p].frost_power = target_fps * 4;
        GS->projs[p].move_speed *= 2;
      }
      if (volley_is_orbit) {
        GS->projs[p].orbit = (GsProjOrbit){
            .timer = 8 * target_fps,
            .cx = GS->player.x,
            .cy = GS->player.y,
            .radius = GS->player.stats.size * 2,
            .target_radius = GS->player.stats.size * 2 + GetRandomValue(0, 10),
            .angle = GS->projs[p].move_angle,
            .rot_speed = 2,
            .track_player_angle = false,
            // .track_player_angle_offset = GetRandomValue(-16, 16),
        };
        GS->projs[p].despawn_timer += GS->projs[p].orbit.timer;
      }

      FixMove(&GS->projs[p].x, &GS->projs[p].y, FixNew(GS->player.stats.size, 0), GS->projs[p].move_angle);
      // printf("Spawned proj %d\n", i);
      break;
    }
  }
}

void GsSpawnSplitProjs(GameScene* GS, int p) {
  int delta = angle_factor / GS->projs[p].split_fragments;
  angle_t ang = GS->projs[p].move_angle + delta / 2;
  for (int i = 0; i < GS->projs[p].split_fragments; ++i) {
    ang += delta;
    for (int q = 0; q < LENGTHOF(GS->projs); ++q) {
      if (GS->projs[q].exists) {
        continue;
      }
      GS->projs[q] = GS->projs[p];
      GS->projs[q].move_angle = ang;
      GS->projs[q].split_fragments = 0;
      GS->projs[q].size /= 2;
      GS->projs[q].despawn_timer = 60;
      GS->projs[q].pierce = 1;

      if (GS->projs[q].orbit.timer > 0) {
        GS->projs[q].orbit.target_radius += GetRandomValue(0, 20);
        GS->projs[q].orbit.timer += target_fps;
      }

      break;
    }
  }
}

void GsUpdateProjs(GameScene* GS) {
  GS->orbital_proj_count = 0;

  // proj logic
  for (int p = 0; p < LENGTHOF(GS->projs); ++p) {
    if (!GS->projs[p].exists) {
      continue;
    }
    if (GS->projs[p].orbit.timer > 0) {
      ++GS->orbital_proj_count;
    }

    // despawn timer
    --GS->projs[p].despawn_timer;
    if (GS->projs[p].despawn_timer <= 0) {
      // printf("Despawned proj %d\n", p);
      GS->projs[p] = (GsProj){0};
      GS->projs[p].exists = false;
      continue;
    }

    // decrement hit-shape timers and zero hit-shape ids for zeroed timers
    for (int h = 0; h < LENGTHOF(GS->projs[p].hit_shape_timers); ++h) {
      if (GS->projs[p].hit_shape_ids[h] == 0) {
        break;
      }
      if (GS->projs[p].hit_shape_timers[h] > 0) {
        --GS->projs[p].hit_shape_timers[h];
        if (GS->projs[p].hit_shape_timers[h] == 0) {
          GS->projs[p].hit_shape_ids[h] = 0;
        }
      }
    }
    // compact the hit-shape entries
    {
      int d = 0;
      for (int h = 0; h < LENGTHOF(GS->projs[p].hit_shape_timers); ++h) {
        GS->projs[p].hit_shape_ids[d] = GS->projs[p].hit_shape_ids[h];
        GS->projs[p].hit_shape_timers[d] = GS->projs[p].hit_shape_timers[h];
        if (GS->projs[p].hit_shape_ids[h] != 0) {
          ++d;
        }
      }
      while (d < LENGTHOF(GS->projs[p].hit_shape_timers)) {
        GS->projs[p].hit_shape_ids[d] = 0;
        GS->projs[p].hit_shape_timers[d] = 0;
        ++d;
      }
    }

    // homing
    GS->projs[p].is_homing = false;
    if (GS->projs[p].homing_max_dist > 0 && GS->projs[p].homing_power > 0) {
      // find nearest eligible shape for homing
      int nearest_shape_idx = -1;
      int nearest_shape_sqdist = INT_MAX;
      for (int s = 0; s < LENGTHOF(GS->shapes); ++s) {
        if (!GS->shapes[s].exists) {
          continue;
        }

        // ignore recently-hit shapes
        bool recently_hit = false;
        for (int h = 0; h < LENGTHOF(GS->projs[p].hit_shape_ids); ++h) {
          if (GS->projs[p].hit_shape_ids[h] == GS->shapes[s].id) {
            recently_hit = true;
            break;
          }
        }
        if (recently_hit) {
          continue;
        }

        // ignore shapes outside of player sight range
        if (GS->shapes[s].sqdist_to_player > IntSq(GS->player.stats.sight_range)) {
          continue;
        }

        int dx = FixWhole(GS->shapes[s].x) - FixWhole(GS->projs[p].x);
        int dy = FixWhole(GS->shapes[s].y) - FixWhole(GS->projs[p].y);
        int sqdist = IntSq(dx) + IntSq(dy);
        if (sqdist < nearest_shape_sqdist) {
          nearest_shape_sqdist = sqdist;
          nearest_shape_idx = s;
        }
      }
      // home, then update homing power
      if (nearest_shape_idx != -1 && nearest_shape_sqdist < IntSq(GS->projs[p].homing_max_dist + GS->shapes[nearest_shape_idx].size)) {
        angle_t target_angle = AngFromLine(GS->projs[p].x, GS->projs[p].y, GS->shapes[nearest_shape_idx].x, GS->shapes[nearest_shape_idx].y);

        // home
        GS->projs[p].is_homing = true;
        AngRotateTowards(&GS->projs[p].move_angle, target_angle, 8);
        --GS->projs[p].homing_power;

        // make text fx
        int t = GsGetTextFxSlot(GS);
        GS->text_fx[t].x = GS->projs[p].x;
        GS->text_fx[t].y = GS->projs[p].y - 64;
        GS->text_fx[t].despawn_timer = 30;
        snprintf(GS->text_fx[t].text, LENGTHOF(GS->text_fx[t].text), ".");
      }
    }

    if (GS->projs[p].orbit.timer > 0) {
      --GS->projs[p].orbit.timer;
    }
    // movement
    if (GS->projs[p].orbit.timer == 0 || GS->projs[p].is_homing) {
      // movement using move_speed and move_angle
      FixMove(&GS->projs[p].x, &GS->projs[p].y, GS->projs[p].move_speed, GS->projs[p].move_angle);

    } else {
      // movement using orbit
      if (GS->projs[p].orbit.track_player_angle) {
        AngRotateTowards(&GS->projs[p].orbit.angle, GS->player.angle + GS->projs[p].orbit.track_player_angle_offset, GS->projs[p].orbit.rot_speed);
      } else {
        GS->projs[p].orbit.angle += GS->projs[p].orbit.rot_speed;
      }

      GS->projs[p].move_angle = GS->projs[p].orbit.angle;
      IntNudge(&GS->projs[p].orbit.radius, GS->projs[p].orbit.target_radius, 1);
      GS->projs[p].orbit.cx = GS->player.x;
      GS->projs[p].orbit.cy = GS->player.y;
      FixLerp(&GS->projs[p].x, GS->projs[p].orbit.cx + FixCos(GS->projs[p].orbit.angle) * GS->projs[p].orbit.radius, FixNew(0, 24));
      FixLerp(&GS->projs[p].y, GS->projs[p].orbit.cy + FixSin(GS->projs[p].orbit.angle) * GS->projs[p].orbit.radius, FixNew(0, 24));
    }

    // shape collision
    bool hit_something = false;
    for (int s = 0; s < LENGTHOF(GS->shapes); ++s) {
      if (!GS->shapes[s].exists) {
        continue;
      }
      if (GS->shapes[s].i_frames > 0) {
        continue;
      }
      int dx = abs(FixWhole(GS->shapes[s].x) - FixWhole(GS->projs[p].x));
      int dy = abs(FixWhole(GS->shapes[s].y) - FixWhole(GS->projs[p].y));
      int sqdist = IntSq(dx) + IntSq(dy);
      if (sqdist >= IntSq(GS->shapes[s].size + GS->projs[p].size)) {
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
          GS->projs[p].hit_shape_timers[h] = 15;
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
      GS->shapes[s].frost_ticks = IntMax(GS->shapes[s].frost_ticks, GS->projs[p].frost_power);
      GS->shapes[s].kb_angle = GS->projs[p].move_angle;
      GS->shapes[s].kb_speed = FixMax(GS->shapes[s].kb_speed, GS->projs[p].kb * GS->shapes[s].max_move_speed / fixed_factor);

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
      int dx = FixWhole(GS->player.x) - FixWhole(GS->pickups[p].x);
      int dy = FixWhole(GS->player.y) - FixWhole(GS->pickups[p].y);
      GS->pickups[p].sqdist_to_player = IntSq(dx) + IntSq(dy);
      GS->pickups[p].angle_to_player = AngFromSlope(dx, dy);
    }

    if (GS->pickups[p].sqdist_to_player < IntSq(GS->player.stats.magnetism_dist)) {
      FixMove(&GS->pickups[p].x, &GS->pickups[p].y, GS->player.stats.max_move_speed, GS->pickups[p].angle_to_player);
    }

    // move faraway off-screen items closer
    if (GS->pickups[p].sqdist_to_player > IntSq(render_w * 2)) {
      FixMove(&GS->pickups[p].x, &GS->pickups[p].y, FixNew(render_w, 0), GS->pickups[p].angle_to_player);
    }
  }
}

void GsUpdateTextFx(GameScene* GS) {
  for (int t = 0; t < LENGTHOF(GS->text_fx); ++t) {
    if (!GS->text_fx[t].exists) {
      continue;
    }
    --GS->text_fx[t].despawn_timer;
    GS->text_fx[t].y -= FixNew(0, 128);
    if (GS->text_fx[t].despawn_timer <= 0) {
      GS->text_fx[t].exists = false;
    }
  }
}

void GsLevelUpPlayer(GameScene* GS) {
  GS->player.xp -= GsXpForLevelUp(GS);
  // GsSpawnPickup(GS, GS->player.x, GS->player.y - FixNew(60, 0));
  ++GS->player.level;
  ++GS->player.upgrades_pending;
}

void GsUpdateXpOrbs(GameScene* GS) {
  for (int o = 0; o < LENGTHOF(GS->xp_orbs); ++o) {
    if (!GS->xp_orbs[o].exists) {
      continue;
    }

    FixMove(&GS->xp_orbs[o].x, &GS->xp_orbs[o].y, GS->xp_orbs[o].move_speed, GS->xp_orbs[o].angle);
    ++GS->xp_orbs[o].age;

    int dx = FixWhole(GS->player.x) - FixWhole(GS->xp_orbs[o].x);
    int dy = FixWhole(GS->player.y) - FixWhole(GS->xp_orbs[o].y);
    angle_t target_angle = AngFromSlope(dx, dy);
    AngRotateTowards(&GS->xp_orbs[o].angle, target_angle, 4);

    int sqdist_to_player = IntSq(dx) + IntSq(dy);
    if (GS->player.ticks_since_magnetism == 0 &&
        sqdist_to_player < IntSq(GS->player.stats.magnetism_dist)) {
      GS->xp_orbs[o].noticed_player = true;
    }
    if (sqdist_to_player < IntSq(GS->player.stats.size + GS->xp_orbs[o].xp / 2 + 12)) {
      GS->xp_orbs[o].noticed_player = true;
    }

    angle_t angle_to_player = AngFromLine(GS->xp_orbs[o].x, GS->xp_orbs[o].y, GS->player.x, GS->player.y);

    // move faraway off-screen orbs closer
    if (sqdist_to_player > IntSq(render_w * 2)) {
      FixMove(&GS->xp_orbs[o].x, &GS->xp_orbs[o].y, FixNew(render_w, 0), angle_to_player);
    }

    if (GS->xp_orbs[o].noticed_player) {
      if (AngDiff(GS->xp_orbs[o].angle, target_angle) > 15) {
        FixNudge(&GS->xp_orbs[o].move_speed, 0, FixNew(0, 16));
      } else {
        FixNudge(&GS->xp_orbs[o].move_speed, FixNew(256, 0) / target_fps, FixNew(0, 16));
      }

      if (sqdist_to_player < IntSq(GS->player.stats.size)) {
        GS->player.total_xp += GS->xp_orbs[o].xp;
        GS->player.xp += GS->xp_orbs[o].xp;
        if (GS->player.xp >= GsXpForLevelUp(GS)) {
          GsLevelUpPlayer(GS);
        }
        // for (int o2 = 0; o2 < LENGTHOF(GS->xp_orbs); ++o2) {
        //   if (!GS->xp_orbs[o2].exists) {
        //     continue;
        //   }
        //   GS->xp_orbs[o2].noticed_player = true;
        // }

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
  FixNudge(&GS->camera.zoom, target_zoom, 1);
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
    --GS->player.upgrades_pending;
    if (GS->player.upgrades_pending > 0) {
      GsOfferUpgrade(GS);
    } else {
      GS->curr_overlay = GS_OVERLAY_NONE;
    }
  }
}

void GsInit(GameScene* GS) {
  // for (int i = 0; i < ITEM_COUNT; ++i) GS->player.item_counts[i] = 1;
  // GS->player.item_counts[ITEM_ORBITALS_UP] = 8;
  GS->player.item_counts[ITEM_HOMING_POWER] = 8;
  // GS->player.item_counts[ITEM_SHOT_SPEED_UP] = 8;
  GS->player.item_counts[ITEM_PIERCE_UP] = 8;
  // GS->player.item_counts[ITEM_FIRE_RATE_UP] = 8;
  GsUpdatePlayerStats(GS, &GS->player.stats);
  GS->player.hp = GS->player.stats.max_hp;
  GS->camera.zoom = FixNew(0, 128);
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
