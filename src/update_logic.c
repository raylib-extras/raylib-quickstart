#include "update_logic.h"

#include <stdlib.h>
#include <string.h>

uint8_t next_free_id = 1;
int ClaimEmptyShapeSlot(GameData* GD) {
  if (GD->shape_count >= LENGTHOF(GD->shapes)) {
    return -1;
  }
  for (int i = 0; i < LENGTHOF(GD->shapes); ++i) {
    if (!GD->shapes[i].exists) {
      ++GD->shape_count;
      GD->shapes[i] = (Shape){0};
      GD->shapes[i].exists = true;
      GD->shapes[i].id = next_free_id;
      ++next_free_id;
      if (next_free_id == 0) {  // skip 0
        ++next_free_id;
      }
      return i;
    }
  }
  return -1;
}

int PickShapeSides(GameData* GD) {
  int r = GetRandomValue(0, 99);
  if (r < 70) {
    return 3;
  } else if (r < 85) {
    return 4;
  } else if (r < 95) {
    return 5;
  } else {
    return 6;
  }
}

ShapeVariant PickShapeVariant(GameData* GD) {
  if (GD->pickups_spawned < 10) {
    return SHAPE_VARIANT_NONE;
  } else if (GD->pickups_spawned < 25) {
    int r = GetRandomValue(0, 99);
    if (r < 90) {
      return SHAPE_VARIANT_NONE;
    } else if (r < 95) {
      return SHAPE_VARIANT_BIG;
    } else {
      return SHAPE_VARIANT_FAST;
    }
  } else {
    int r = GetRandomValue(0, 99);
    if (r < 70) {
      return SHAPE_VARIANT_NONE;
    } else if (r < 80) {
      return SHAPE_VARIANT_BIG;
    } else if (r < 90) {
      return SHAPE_VARIANT_FAST;
    } else {
      return SHAPE_VARIANT_HEALING;
    }
  }
}

void SetShapeStats(Shape* shape, int sides, ShapeVariant variant) {
  shape->sides = sides;
  shape->variant = variant;
  switch (sides) {
    case 3: {
      shape->max_hp = 200;
      shape->max_move_speed = fixed_new(64, 0) / target_fps;
      shape->regen = 0;
      shape->sides = 3;
      shape->size = 8;
      shape->fg = YELLOW;
      shape->bg = GOLD;
      shape->spawn_children_on_despawn = false;
    } break;
    case 4: {
      shape->max_hp = 400;
      shape->max_move_speed = fixed_new(24, 0) / target_fps;
      shape->regen = 4;
      shape->sides = 4;
      shape->size = 12;
      shape->fg = PINK;
      shape->bg = RED;
      shape->spawn_children_on_despawn = true;
    } break;
    case 5: {
      shape->max_hp = 1000;
      shape->max_move_speed = fixed_new(16, 0) / target_fps;
      shape->regen = 12;
      shape->sides = 5;
      shape->size = 24;
      shape->fg = PURPLE;
      shape->bg = VIOLET;
      shape->spawn_children_on_despawn = true;
    } break;
    case 6: {
      shape->max_hp = 5000;
      shape->max_move_speed = fixed_new(16, 0) / target_fps;
      shape->regen = 50;
      shape->sides = 6;
      shape->size = 40;
      shape->fg = GREEN;
      shape->bg = LIME;
      shape->spawn_children_on_despawn = true;
    } break;
    default: {
      TraceLog(LOG_ERROR, "Bad Shape side count of %d", sides);
    } break;
  }

  switch (variant) {
    case SHAPE_VARIANT_NONE: {
    } break;
    case SHAPE_VARIANT_BIG: {
      shape->fg = DARKGRAY;
      shape->bg = BLACK;
      shape->size *= 2;
      shape->max_hp *= 4;
      shape->regen /= 2;
      shape->max_move_speed;
    } break;
    case SHAPE_VARIANT_FAST: {
      shape->fg = ORANGE;
      shape->bg = BLACK;
      shape->max_hp *= 2;
      shape->regen = 0;
      shape->max_move_speed *= 3;
    } break;
    case SHAPE_VARIANT_HEALING: {
      shape->fg = RED;
      shape->bg = WHITE;
      shape->max_hp *= 2;
      shape->regen += 10;
    } break;
    default: {
      TraceLog(LOG_ERROR, "Bad Shape side count of %d", sides);
    } break;
  }

  shape->hp = shape->max_hp;
  shape->marked_for_despawn = 0;
}

void SpawnChildShapes(GameData* GD, int parent) {
  int parent_sides = GD->shapes[parent].sides;
  angle_t ang = GetRandomValue(0, angle_factor - 1);
  for (int i = 0; i < parent_sides; ++i) {
    ang += (angle_factor / parent_sides);
    int s = ClaimEmptyShapeSlot(GD);
    if (s == -1) {
      return;
    }
    GD->shapes[s].x = GD->shapes[parent].x;
    GD->shapes[s].y = GD->shapes[parent].y;
    GD->shapes[s].kb_speed = fixed_new(75, 0) / target_fps;
    GD->shapes[s].kb_angle = ang;
    GD->shapes[s].i_frames = 30;
    GD->shapes[s].ticks_since_damaged = 0;
    SetShapeStats(&GD->shapes[s], parent_sides - 1, GD->shapes[parent].variant);
    GD->shapes[s].hp = GD->shapes[s].max_hp / 2;
  }
}

void SpawnPickup(GameData* GD, int s) {
  for (int p = 0; p < LENGTHOF(GD->pickups); ++p) {
    if (GD->pickups[p].exists) {
      continue;
    }
    GD->pickups[p].exists = true;
    GD->pickups[p].x = GD->shapes[s].x;
    GD->pickups[p].y = GD->shapes[s].y;
    GD->pickups[p].type = GetRandomValue(0, ITEM_COUNT - 1);
    if (GD->player.item_counts[GD->pickups[p].type] >= 8) {
      GD->pickups[p].exists = false;
    }
    return;
  }
}

void UpdatePlayerStats(GameData* GD) {
  GD->player.stat_update_timer = 4 * target_fps;

  GD->player.stats.damage = 40;
  GD->player.stats.max_move_speed = fixed_new(1, 128);
  GD->player.stats.reload_delay = fixed_new(0, 128);
  GD->player.stats.shot_count = fixed_new(1, 0);
  GD->player.stats.shot_kb = fixed_new(0, 0);
  GD->player.stats.shot_pierce = 1;
  GD->player.stats.shot_speed = fixed_new(2, 0);
  GD->player.stats.shot_spread = 1;
  GD->player.stats.sight_range = render_h / 2;
  GD->player.stats.size = 12;
  GD->player.stats.turn_speed = 2;
  GD->player.stats.magnetism_dist = 16;
  GD->player.stats.shot_homing_power = 0;
  GD->player.stats.view_distance = 120;
  for (int i = 0; i < ITEM_COUNT; ++i) {
    for (int x = 0; x < GD->player.item_counts[i]; ++x) {
      switch (i) {
        case ITEM_SPEED_UP:
          GD->player.stats.max_move_speed += fixed_new(0, 64);
          break;
        case ITEM_FIRE_RATE_UP:
          GD->player.stats.reload_delay = GD->player.stats.reload_delay * 80 / 100;
          GD->player.stats.shot_spread += 2;
          break;
        case ITEM_TURN_SPEED_UP:
          ++GD->player.stats.turn_speed;
          break;
        case ITEM_DAMAGE_UP:
          GD->player.stats.damage += 10;
          GD->player.stats.reload_delay = GD->player.stats.reload_delay * 105 / 100;
          break;
        case ITEM_ACCURACY_UP:
          GD->player.stats.shot_spread -= 2;
          break;
        case ITEM_SHOT_SPEED_UP:
          GD->player.stats.shot_speed += fixed_new(0, 128);
          GD->player.stats.damage = GD->player.stats.damage * 105 / 100;
          GD->player.stats.shot_kb += fixed_new(0, 16);
          break;
        case ITEM_SHOT_COUNT_UP:
          GD->player.stats.shot_count += fixed_new(0, 64);
          GD->player.stats.reload_delay = GD->player.stats.reload_delay * 105 / 100;
          GD->player.stats.shot_spread += 2;
          break;
        case ITEM_SHOT_KB_UP:
          GD->player.stats.shot_kb += fixed_new(0, 64);
          break;
        case ITEM_PIERCE_UP:
          GD->player.stats.shot_pierce += 1;
          if (GD->player.stats.shot_pierce < 4) {
            GD->player.stats.reload_delay = GD->player.stats.reload_delay * 115 / 100;
          }
          break;
        case ITEM_MAGNETISM_UP:
          GD->player.stats.magnetism_dist += 16;
          break;
        case ITEM_HOMING_POWER:
          GD->player.stats.shot_homing_power += 8;
          break;
        case ITEM_SIGHT_UP:
          GD->player.stats.sight_range += 12;
          GD->player.stats.view_distance += 12;
          break;
        default:
          TraceLog(LOG_WARNING, "Unhandled item with id %d", i);
      }
    }
  }

  clamp(&GD->player.stats.damage, 10, 1000);
  fixed_clamp(&GD->player.stats.max_move_speed, fixed_new(1, 0), fixed_new(4, 0));
  fixed_clamp(&GD->player.stats.reload_delay, fixed_new(0, 8), fixed_new(5, 0));
  fixed_clamp(&GD->player.stats.shot_count, fixed_new(1, 0), fixed_new(8, 0));
  fixed_clamp(&GD->player.stats.shot_kb, fixed_new(0, 0), fixed_new(4, 0));
  clamp(&GD->player.stats.shot_pierce, 1, 8);
  fixed_clamp(&GD->player.stats.shot_speed, fixed_new(0, 128), fixed_new(8, 0));
  clamp(&GD->player.stats.shot_spread, 0, 32);
  clamp(&GD->player.stats.size, 10, 50);
  clamp(&GD->player.stats.turn_speed, 1, 64);
  clamp(&GD->player.stats.magnetism_dist, 16, 1000);
  clamp(&GD->player.stats.view_distance, 120, 240);
}

void SpawnNewShapes(GameData* GD) {
  if (GD->ticks % 30 == 0 && GD->shape_count < (LENGTHOF(GD->shapes) / 2)) {
    int s = ClaimEmptyShapeSlot(GD);
    if (s == -1) {
      return;
    }
    GD->shapes[s].x = GD->player.x + fixed_new(GetRandomValue(-render_w, render_w), 0);
    GD->shapes[s].y = GD->player.y + fixed_new(GetRandomValue(-render_h, render_h), 0);
    GD->shapes[s].move_angle = GetRandomValue(0, angle_factor - 1);
    GD->shapes[s].ticks_since_damaged = 1000;
    SetShapeStats(&GD->shapes[s], PickShapeSides(GD), PickShapeVariant(GD));
    // printf("Spawned shape %d\n", s);
  }
}

int CompareShapes(const void* p, const void* q) {
  const Shape* a = p;
  const Shape* b = q;
  int exists_cmp = a->exists - b->exists;
  if (exists_cmp != 0) {
    return -exists_cmp;
  }
  return (a->y - b->y);
}

int GetTextFxSlot(GameData* GD) {
  int oldest_idx = 0;
  for (int t = 0; t < LENGTHOF(GD->text_fx); ++t) {
    if (!GD->text_fx[t].exists) {
      GD->text_fx[t].exists = true;
      return t;
    }
    if (GD->text_fx[t].despawn_timer < GD->text_fx[oldest_idx].despawn_timer) {
      oldest_idx = t;
    }
  }

  // force-override oldest text effect if there were no slots available
  GD->text_fx[oldest_idx] = (TextFx){0};
  GD->text_fx[oldest_idx].exists = true;
  return oldest_idx;
}

void InitGameData(GameData* GD) {
  // for (int c = 0; c < LENGTHOF(GD->player.item_counts); ++c) {
  //   GD->player.item_counts[c] = 6;
  // }
  *GD = (GameData){0};
  UpdatePlayerStats(GD);
  GD->camera.zoom = fixed_new(1, 0);
  GD->font = LoadFontEx("Kitchen Sink.ttf", 8, NULL, 0);
}

void UpdateShapes(GameData* GD) {
  // shape logic
  for (int s = 0; s < LENGTHOF(GD->shapes); ++s) {
    if (!GD->shapes[s].exists) {
      continue;
    }

    // hp and damage-related
    if (GD->shapes[s].hp <= 0) {
      GD->shapes[s].marked_for_despawn = true;
    }
    ++GD->shapes[s].ticks_since_damaged;
    if (GD->shapes[s].i_frames > 0) {
      --GD->shapes[s].i_frames;
    }
    if (GD->ticks % 60 < GD->shapes[s].regen && (GD->shapes[s].hp < GD->shapes[s].max_hp)) {
      GD->shapes[s].hp += 10;
    }
    if (GD->shapes[s].variant == SHAPE_VARIANT_HEALING && (GD->shapes[s].hp < GD->shapes[s].max_hp)) {
      GD->shapes[s].hp += GD->shapes[s].max_hp / 120;
    }

    // despawning
    if (GD->shapes[s].marked_for_despawn) {
      // printf("Despawned shape %d\n", i);
      if (GD->shapes[s].sides > 3) {
        if (GD->shapes[s].spawn_children_on_despawn) {
          SpawnChildShapes(GD, s);
        }
      } else {
        if (GD->shapes[s].spawn_pickup_on_despawn) {
          SpawnPickup(GD, s);
        }
      }
      GD->shapes[s] = (Shape){0};
      GD->shapes[s].exists = false;
      --GD->shape_count;
      continue;
    }

    // movement
    fixed_t real_move_speed = GD->shapes[s].move_speed;
    if (GD->shapes[s].sqdist_to_target < int_sq(64) && GD->shapes[s].move_speed > GD->player.stats.max_move_speed) {
      real_move_speed = GD->player.stats.max_move_speed;
    }
    GD->shapes[s].x += fixed_cos(GD->shapes[s].move_angle) * real_move_speed / fixed_factor;
    GD->shapes[s].y += fixed_sin(GD->shapes[s].move_angle) * real_move_speed / fixed_factor;

    // kb
    GD->shapes[s].x += fixed_cos(GD->shapes[s].kb_angle) * GD->shapes[s].kb_speed / fixed_factor;
    GD->shapes[s].y += fixed_sin(GD->shapes[s].kb_angle) * GD->shapes[s].kb_speed / fixed_factor;
    GD->shapes[s].kb_speed -= fixed_new(0, 4);
    fixed_clamp(&GD->shapes[s].kb_speed, 0, fixed_new(64, 0));

    // update sqdist_to_player, angle_to_player
    {
      int dx = fixed_whole(GD->player.x) - fixed_whole(GD->shapes[s].x);
      int dy = fixed_whole(GD->player.y) - fixed_whole(GD->shapes[s].y);
      GD->shapes[s].sqdist_to_player = int_sq(dx) + int_sq(dy);
      GD->shapes[s].angle_to_player = angle_from_slope(dx, dy);
    }

    // targeting
    int aggro_time = 180;
    if (GD->shapes[s].variant == SHAPE_VARIANT_FAST) {
      aggro_time = 600;
    }
    if (GD->shapes[s].always_target || GD->shapes[s].ticks_since_damaged < aggro_time) {
      // update target_x, target_y
      GD->shapes[s].target_x = GD->player.x;
      GD->shapes[s].target_y = GD->player.y;

      // update sqdist_to_target, angle_to_target
      {
        int dx = fixed_whole(GD->shapes[s].target_x) - fixed_whole(GD->shapes[s].x);
        int dy = fixed_whole(GD->shapes[s].target_y) - fixed_whole(GD->shapes[s].y);
        GD->shapes[s].sqdist_to_target = int_sq(dx) + int_sq(dy);
        GD->shapes[s].angle_to_target = angle_from_slope(dx, dy);
      }

      // FAST variants have a slower turn speed
      if (GD->shapes[s].variant == SHAPE_VARIANT_FAST) {
        angle_rotate_towards(&GD->shapes[s].move_angle, GD->shapes[s].angle_to_target, 2);
      } else {
        angle_rotate_towards(&GD->shapes[s].move_angle, GD->shapes[s].angle_to_target, 3);
      }

      // shapes use a slower max move speed when not facing target
      if (abs(angle_diff(GD->shapes[s].move_angle, GD->shapes[s].angle_to_target)) > 15) {
        fixed_nudge(&GD->shapes[s].move_speed, GD->shapes[s].max_move_speed / 2, fixed_new(0, 16));
      } else {
        fixed_nudge(&GD->shapes[s].move_speed, GD->shapes[s].max_move_speed, fixed_new(0, 8));
      }
    } else {
      fixed_nudge(&GD->shapes[s].move_speed, GD->shapes[s].max_move_speed / 2, fixed_new(0, 8));
    }

    // player distance-related despawning
    if (GD->shapes[s].sqdist_to_player > int_sq(render_w)) {
      GD->shapes[s].marked_for_despawn = true;
      GD->shapes[s].spawn_children_on_despawn = false;
    }

    if (GD->ticks % 10 == 0) {
      // shape-shape collision
      for (int j = s + 1; j < LENGTHOF(GD->shapes); ++j) {
        if (!GD->shapes[j].exists) {
          continue;
        }
        int dx = abs(fixed_whole(GD->shapes[s].x) - fixed_whole(GD->shapes[j].x));
        int dy = abs(fixed_whole(GD->shapes[s].y) - fixed_whole(GD->shapes[j].y));
        int sqdist = int_sq(dx) + int_sq(dy);
        if (sqdist < int_sq((GD->shapes[s].size + GD->shapes[j].size) / 2)) {
          if (GD->shapes[s].x < GD->shapes[j].x) {
            GD->shapes[s].x -= fixed_new(1, 0);
            GD->shapes[j].x += fixed_new(1, 0);
          } else {
            GD->shapes[s].x += fixed_new(1, 0);
            GD->shapes[j].x -= fixed_new(1, 0);
          }
        }
      }
    }
  }
  qsort(GD->shapes, LENGTHOF(GD->shapes), sizeof(Shape), CompareShapes);
  // for (int s = 0; s < LENGTHOF(GD->shapes); ++s) {
  //   if (GD->shapes[s].exists) {
  //     printf("%c", GD->shapes[s].sides + '0');

  //   } else {
  //     printf("_");
  //   }
  // }
  // printf(" %d\n", GD->shape_count);
}

void UpdatePlayer(GameData* GD) {
  // player movement
  GD->player.move_speed = GD->player.stats.max_move_speed;
  if (IsKeyDown(KEY_A)) GD->player.x -= GD->player.move_speed;
  if (IsKeyDown(KEY_D)) GD->player.x += GD->player.move_speed;
  if (IsKeyDown(KEY_S)) GD->player.y += GD->player.move_speed;
  if (IsKeyDown(KEY_W)) GD->player.y -= GD->player.move_speed;

  // find index of closest shape
  int closest_sqdist = INT_MAX;
  int closest_shape_idx = -1;
  for (int s = 0; s < LENGTHOF(GD->shapes); ++s) {
    if (GD->shapes[s].exists) {
      if (GD->shapes[s].sqdist_to_player < int_sq(GD->player.stats.sight_range) &&
          GD->shapes[s].sqdist_to_player < closest_sqdist) {
        closest_shape_idx = s;
        closest_sqdist = GD->shapes[s].sqdist_to_player;
      }
    }
  }
  // rotate player towards closest shape
  if (closest_shape_idx != -1) {
    angle_t target_angle = angle_from_line(GD->player.x, GD->player.y, GD->shapes[closest_shape_idx].x, GD->shapes[closest_shape_idx].y);
    angle_rotate_towards(&GD->player.angle, target_angle, GD->player.stats.turn_speed);
  }

  // collect pickups
  for (int p = 0; p < LENGTHOF(GD->pickups); ++p) {
    if (!GD->pickups[p].exists) {
      continue;
    }
    int dx = abs(fixed_whole(GD->pickups[p].x) - fixed_whole(GD->player.x));
    int dy = abs(fixed_whole(GD->pickups[p].y) - fixed_whole(GD->player.y));
    int sqdist = int_sq(dx) + int_sq(dy);
    if (sqdist < int_sq(GD->player.stats.size)) {
      GD->pickups[p].exists = false;
      ++GD->player.item_counts[GD->pickups[p].type];
      UpdatePlayerStats(GD);
    }
  }

  // update dps calculation
  int next_entry = (GD->ticks + 1) % LENGTHOF(GD->player.damage_history);
  int curr_entry = (GD->ticks) % LENGTHOF(GD->player.damage_history);
  GD->player.dps -= GD->player.damage_history[next_entry];
  GD->player.damage_history[next_entry] = 0;

  // update stat update timer
  if (GD->player.stat_update_timer > 0) {
    --GD->player.stat_update_timer;
    if (GD->player.stat_update_timer == 0) {
      GD->player.prev_stats = GD->player.stats;
    }
  }
}

void SpawnNewProjs(GameData* GD) {
  // spawn new projs
  GD->player.reload_progress += fixed_factor * fixed_factor / target_fps / GD->player.stats.reload_delay;
  while (GD->player.reload_progress >= fixed_factor) {
    GD->player.reload_progress -= fixed_factor;
    GD->player.shot_progress += GD->player.stats.shot_count;
  }
  bool first_shot_in_volley = true;
  while (GD->player.shot_progress >= fixed_new(1, 0)) {
    GD->player.shot_progress -= fixed_new(1, 0);
    for (int p = 0; p < LENGTHOF(GD->projs); ++p) {
      if (GD->projs[p].exists) {
        continue;
      }
      GD->projs[p].exists = true;
      GD->projs[p].pierce = GD->player.stats.shot_pierce;
      GD->projs[p].x = GD->player.x;
      GD->projs[p].y = GD->player.y;
      GD->projs[p].homing_max_dist = 64;
      GD->projs[p].homing_power = GD->player.stats.shot_homing_power;
      if (first_shot_in_volley) {
        GD->projs[p].move_speed = GD->player.stats.shot_speed;
        GD->projs[p].angle = GD->player.angle + GetRandomValue(-GD->player.stats.shot_spread, GD->player.stats.shot_spread);
        GD->projs[p].size = 6 + (GD->player.stats.damage / 40);
        GD->projs[p].damage = GD->player.stats.damage;
        GD->projs[p].despawn_timer = 120;
        GD->projs[p].kb = GD->player.stats.shot_kb;
        first_shot_in_volley = false;
      } else {
        GD->projs[p].move_speed = GD->player.stats.shot_speed * GetRandomValue(50, 90) / 100;
        GD->projs[p].angle = GD->player.angle + GetRandomValue(-GD->player.stats.shot_spread * 2, GD->player.stats.shot_spread * 2);
        GD->projs[p].size = 6 + (GD->player.stats.damage / 80);
        GD->projs[p].damage = GD->player.stats.damage / 2;
        GD->projs[p].despawn_timer = 120;
        GD->projs[p].kb = GD->player.stats.shot_kb / 2;
      }
      // printf("Spawned proj %d\n", i);
      break;
    }
  }
}

void UpdateProjs(GameData* GD) {
  // proj logic
  for (int p = 0; p < LENGTHOF(GD->projs); ++p) {
    if (!GD->projs[p].exists) {
      continue;
    }
    // despawn timer
    --GD->projs[p].despawn_timer;
    if (GD->projs[p].despawn_timer <= 0) {
      // printf("Despawned proj %d\n", p);
      GD->projs[p] = (Proj){0};
      GD->projs[p].exists = false;
      continue;
    }

    // homing
    GD->projs[p].is_homing = false;
    if (GD->projs[p].homing_max_dist > 0 && GD->projs[p].homing_power > 0) {
      // find nearest shape we haven't hit yet
      int nearest_shape_idx = -1;
      int s_sqdist = INT_MAX;
      for (int s = 0; s < LENGTHOF(GD->shapes); ++s) {
        if (!GD->shapes[s].exists) {
          continue;
        }
        bool already_hit = false;
        for (int h = 0; h < LENGTHOF(GD->projs[p].hit_shape_ids); ++h) {
          if (GD->projs[p].hit_shape_ids[h] == GD->shapes[s].id) {
            already_hit = true;
            break;
          }
        }
        if (already_hit) {
          continue;
        }
        int dx = fixed_whole(GD->shapes[s].x) - fixed_whole(GD->projs[p].x);
        int dy = fixed_whole(GD->shapes[s].y) - fixed_whole(GD->projs[p].y);
        int sqdist = int_sq(dx) + int_sq(dy);
        if (sqdist < s_sqdist) {
          s_sqdist = sqdist;
          nearest_shape_idx = s;
        }
      }
      // home, then update homing power
      if (nearest_shape_idx != -1 && s_sqdist < int_sq(GD->projs[p].homing_max_dist + GD->shapes[nearest_shape_idx].size)) {
        angle_t target_angle = angle_from_line(GD->projs[p].x, GD->projs[p].y, GD->shapes[nearest_shape_idx].x, GD->shapes[nearest_shape_idx].y);
        if (target_angle != GD->projs[p].angle) {
          GD->projs[p].is_homing = true;
          angle_rotate_towards(&GD->projs[p].angle, target_angle, 8);
          --GD->projs[p].homing_power;

          // make text fx
          int t = GetTextFxSlot(GD);
          GD->text_fx[t].x = GD->projs[p].x;
          GD->text_fx[t].y = GD->projs[p].y - 64;
          GD->text_fx[t].despawn_timer = 30;
          snprintf(GD->text_fx[t].text, LENGTHOF(GD->text_fx[t].text), ".");
        }
      }
    }

    // movement
    GD->projs[p].x += fixed_cos(GD->projs[p].angle) * GD->projs[p].move_speed / fixed_factor;
    GD->projs[p].y += fixed_sin(GD->projs[p].angle) * GD->projs[p].move_speed / fixed_factor;

    // shape collision
    for (int s = 0; s < LENGTHOF(GD->shapes); ++s) {
      if (!GD->shapes[s].exists) {
        continue;
      }
      if (GD->shapes[s].i_frames > 0) {
        continue;
      }
      int dx = abs(fixed_whole(GD->shapes[s].x) - fixed_whole(GD->projs[p].x));
      int dy = abs(fixed_whole(GD->shapes[s].y) - fixed_whole(GD->projs[p].y));
      int sqdist = int_sq(dx) + int_sq(dy);
      if (sqdist >= int_sq(GD->shapes[s].size + GD->projs[p].size)) {
        continue;
      }
      // see if shape has been hit already
      bool skip_this_shape = false;
      for (int h = 0; h < LENGTHOF(GD->projs[p].hit_shape_ids); ++h) {
        if (GD->projs[p].hit_shape_ids[h] == GD->shapes[s].id) {
          // found shape in hit-shapes list
          skip_this_shape = true;
          break;
        }
        if (GD->projs[p].hit_shape_ids[h] == 0) {
          // reached end of hit-shapes list, add new shape
          GD->projs[p].hit_shape_ids[h] = GD->shapes[s].id;
          break;
        }
      }
      if (skip_this_shape) {
        continue;
      }

      // damage the shape
      GD->shapes[s].hp -= GD->projs[p].damage;
      GD->shapes[s].ticks_since_damaged = 0;
      GD->shapes[s].kb_angle = GD->projs[p].angle;
      GD->shapes[s].kb_speed = GD->projs[p].kb * GD->shapes[s].max_move_speed / fixed_factor;

      // make text fx
      int t = GetTextFxSlot(GD);
      GD->text_fx[t].x = GD->shapes[s].x;
      GD->text_fx[t].y = GD->shapes[s].y;
      GD->text_fx[t].despawn_timer = 60;
      snprintf(GD->text_fx[t].text, LENGTHOF(GD->text_fx[t].text), "-%d", GD->projs[p].damage);

      // do player dps tracking
      GD->player.dps += GD->projs[p].damage;
      GD->player.damage_history[GD->ticks % LENGTHOF(GD->player.damage_history)] += GD->projs[p].damage;
      if (GD->shapes[s].hp <= 0 && GetRandomValue(0, (GD->pickups_spawned <= 5 ? 1 : 5)) == 0) {
        GD->shapes[s].spawn_pickup_on_despawn = true;
        ++GD->pickups_spawned;
      }

      // queue proj for despawn
      GD->projs[p].pierce -= 1;
      if (GD->projs[p].pierce <= 0) {
        GD->projs[p].despawn_timer = 0;
        break;
      }
    }
  }
}

void UpdatePickups(GameData* GD) {
  for (int p = 0; p < LENGTHOF(GD->pickups); ++p) {
    if (!GD->pickups[p].exists) {
      continue;
    }

    // update sqdist_to_player
    {
      int dx = fixed_whole(GD->player.x) - fixed_whole(GD->pickups[p].x);
      int dy = fixed_whole(GD->player.y) - fixed_whole(GD->pickups[p].y);
      GD->pickups[p].sqdist_to_player = int_sq(dx) + int_sq(dy);
      GD->pickups[p].angle_to_player = angle_from_slope(dx, dy);
    }

    if (GD->pickups[p].sqdist_to_player < int_sq(GD->player.stats.magnetism_dist)) {
      GD->pickups[p].x += fixed_cos(GD->pickups[p].angle_to_player) * GD->player.stats.max_move_speed / fixed_factor;
      GD->pickups[p].y += fixed_sin(GD->pickups[p].angle_to_player) * GD->player.stats.max_move_speed / fixed_factor;
    }

    // move faraway off-screen items closer
    if (GD->pickups[p].sqdist_to_player > int_sq(render_w * 2)) {
      GD->pickups[p].x += fixed_cos(GD->pickups[p].angle_to_player) * render_w;
      GD->pickups[p].y += fixed_sin(GD->pickups[p].angle_to_player) * render_w;
    }
  }
}

void UpdateTextFx(GameData* GD) {
  for (int t = 0; t < LENGTHOF(GD->text_fx); ++t) {
    if (!GD->text_fx[t].exists) {
      continue;
    }
    --GD->text_fx[t].despawn_timer;
    GD->text_fx[t].y -= fixed_new(0, 128);
    if (GD->text_fx[t].despawn_timer <= 0) {
      GD->text_fx[t].exists = false;
    }
  }
}
