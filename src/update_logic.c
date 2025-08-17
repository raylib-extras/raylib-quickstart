#include "update_logic.h"

#include <stdlib.h>
#include <string.h>

int ClaimEmptyShapeSlot(GameData* GD) {
  if (GD->shape_count >= LENGTHOF(GD->shapes)) {
    return -1;
  }
  for (int i = 0; i < LENGTHOF(GD->shapes); ++i) {
    if (!GD->shapes[i].exists) {
      ++GD->shape_count;
      memset(&GD->shapes[i], 0, sizeof(Shape));
      GD->shapes[i].exists = true;
      return i;
    }
  }
  return -1;
}

void SetShapeStats(Shape* shape, int sides) {
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
      shape->max_move_speed = fixed_new(8, 0) / target_fps;
      shape->regen = 12;
      shape->sides = 5;
      shape->size = 24;
      shape->fg = PURPLE;
      shape->bg = VIOLET;
      shape->spawn_children_on_despawn = true;
    } break;
    case 6: {
      shape->max_hp = 5000;
      shape->max_move_speed = fixed_new(4, 0) / target_fps;
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
    SetShapeStats(&GD->shapes[s], parent_sides - 1);
    GD->shapes[s].hp = GD->shapes[s].max_hp / 2;
  }
}

void SpawnPickup(GameData* GD, int s) {
  const int x = _Generic(s, char: 5, float: 3, default: 4);
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
  GD->player.size = 12;
  GD->player.max_move_speed = fixed_new(1, 128);
  GD->player.reload_delay = fixed_new(0, 128);
  GD->player.sight_range = render_h / 2;
  GD->player.turn_speed = 2;
  GD->player.damage = 45;
  GD->player.shot_spread = 1;
  GD->player.shot_speed = fixed_new(2, 0);
  GD->player.shot_count = fixed_new(1, 0);
  GD->player.shot_kb = fixed_new(0, 0);
  for (int i = 0; i < ITEM_COUNT; ++i) {
    for (int x = 0; x < GD->player.item_counts[i]; ++x) {
      switch (i) {
        case ITEM_SPEED_UP:
          GD->player.max_move_speed += fixed_new(0, 64);
          break;
        case ITEM_FIRE_RATE_UP:
          GD->player.reload_delay = GD->player.reload_delay * 80 / 100;
          GD->player.shot_spread += 2;
          break;
        case ITEM_TURN_SPEED_UP:
          ++GD->player.turn_speed;
          break;
        case ITEM_DAMAGE_UP:
          GD->player.damage = GD->player.damage * 125 / 100;
          GD->player.reload_delay = GD->player.reload_delay * 105 / 100;
          break;
        case ITEM_ACCURACY_UP:
          GD->player.shot_spread -= 2;
          break;
        case ITEM_SHOT_SPEED_UP:
          GD->player.shot_speed += fixed_new(0, 128);
          GD->player.damage = GD->player.damage * 105 / 100;
          GD->player.shot_kb += fixed_new(0, 16);
          break;
        case ITEM_SHOT_COUNT_UP:
          GD->player.shot_count += fixed_new(0, 64);
          GD->player.reload_delay = GD->player.reload_delay * 105 / 100;
          GD->player.shot_spread += 2;
          break;
        case ITEM_SHOT_KB_UP:
          GD->player.shot_kb += fixed_new(0, 64);
          break;
        default:
          TraceLog(LOG_WARNING, "Unhandled item with id %d", i);
      }
    }
  }

  clamp(&GD->player.size, 10, 50);
  fixed_clamp(&GD->player.max_move_speed, fixed_new(1, 0), fixed_new(4, 0));
  fixed_clamp(&GD->player.reload_delay, fixed_new(0, 8), fixed_new(5, 0));
  // clamp(&GD->player.sight_range);
  clamp(&GD->player.turn_speed, 1, 64);
  clamp(&GD->player.damage, 10, 1000);
  clamp(&GD->player.shot_spread, 0, 32);
  fixed_clamp(&GD->player.shot_speed, fixed_new(0, 128), fixed_new(8, 0));
  fixed_clamp(&GD->player.shot_count, fixed_new(1, 0), fixed_new(8, 0));
  fixed_clamp(&GD->player.shot_kb, fixed_new(0, 0), fixed_new(4, 0));
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
    int rand = GetRandomValue(0, 99);
    if (rand < 70) {
      SetShapeStats(&GD->shapes[s], 3);
    } else if (rand < 85) {
      SetShapeStats(&GD->shapes[s], 4);
    } else if (rand < 95) {
      SetShapeStats(&GD->shapes[s], 5);
    } else {
      SetShapeStats(&GD->shapes[s], 6);
    }
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

void InitGameData(GameData* GD) {
  UpdatePlayerStats(GD);
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
      memset(&GD->shapes[s], 0, sizeof(Shape));
      GD->shapes[s].exists = false;
      --GD->shape_count;
      continue;
    }

    // movement
    GD->shapes[s].x += fixed_cos(GD->shapes[s].move_angle) * GD->shapes[s].move_speed / fixed_factor;
    GD->shapes[s].y += fixed_sin(GD->shapes[s].move_angle) * GD->shapes[s].move_speed / fixed_factor;

    // kb
    GD->shapes[s].x += fixed_cos(GD->shapes[s].kb_angle) * GD->shapes[s].kb_speed / fixed_factor;
    GD->shapes[s].y += fixed_sin(GD->shapes[s].kb_angle) * GD->shapes[s].kb_speed / fixed_factor;
    GD->shapes[s].kb_speed -= fixed_new(0, 4);
    fixed_clamp(&GD->shapes[s].kb_speed, 0, fixed_new(64, 0));

    // update sqdist_to_player
    {
      int dx = fixed_whole(GD->player.x) - fixed_whole(GD->shapes[s].x);
      int dy = fixed_whole(GD->player.y) - fixed_whole(GD->shapes[s].y);
      GD->shapes[s].sqdist_to_player = int_sq(dx) + int_sq(dy);
      GD->shapes[s].angle_to_player = angle_from_slope(dx, dy);
    }

    // targeting player
    if (GD->shapes[s].ticks_since_damaged < 180) {
      angle_rotate_towards(&GD->shapes[s].move_angle, GD->shapes[s].angle_to_player, 3);
      fixed_nudge(&GD->shapes[s].move_speed, GD->shapes[s].max_move_speed, fixed_new(0, 8));
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

    // absorption
    // if (GD->shapes[i].sqdist_to_player < int_sq(GD->player.size - 4)) {
    //   GD->player.size += 1;
    //   GD->shapes[i].hp -= 10;
    // } else if (GD->shapes[i].sqdist_to_player < int_sq(GD->player.size + 8)) {
    //   angle_t target_angle = angle_from_line(GD->shapes[i].x, GD->shapes[i].y, GD->player.x, GD->player.y);
    //   GD->shapes[i].angle = target_angle;
    // }
  }
  // qsort(GD->shapes, LENGTHOF(GD->shapes), sizeof(Shape), CompareShapes);
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
  GD->player.move_speed = GD->player.max_move_speed;
  if (IsKeyDown(KEY_A)) GD->player.x -= GD->player.move_speed;
  if (IsKeyDown(KEY_D)) GD->player.x += GD->player.move_speed;
  if (IsKeyDown(KEY_S)) GD->player.y += GD->player.move_speed;
  if (IsKeyDown(KEY_W)) GD->player.y -= GD->player.move_speed;

  // find index of closest shape
  int closest_sqdist = INT32_MAX;
  int closest_shape_idx = -1;
  for (int s = 0; s < LENGTHOF(GD->shapes); ++s) {
    if (GD->shapes[s].exists) {
      if (GD->shapes[s].sqdist_to_player < int_sq(GD->player.sight_range) &&
          GD->shapes[s].sqdist_to_player < closest_sqdist) {
        closest_shape_idx = s;
        closest_sqdist = GD->shapes[s].sqdist_to_player;
      }
    }
  }
  // rotate player towards closest shape
  if (closest_shape_idx != -1) {
    angle_t target_angle = angle_from_line(GD->player.x, GD->player.y, GD->shapes[closest_shape_idx].x, GD->shapes[closest_shape_idx].y);
    angle_rotate_towards(&GD->player.angle, target_angle, GD->player.turn_speed);
  }

  // collect pickups
  for (int p = 0; p < LENGTHOF(GD->pickups); ++p) {
    if (!GD->pickups[p].exists) {
      continue;
    }
    int dx = abs(fixed_whole(GD->pickups[p].x) - fixed_whole(GD->player.x));
    int dy = abs(fixed_whole(GD->pickups[p].y) - fixed_whole(GD->player.y));
    int sqdist = int_sq(dx) + int_sq(dy);
    if (sqdist < int_sq(GD->player.size)) {
      GD->pickups[p].exists = false;
      ++GD->player.item_counts[GD->pickups[p].type];
      UpdatePlayerStats(GD);
    }
  }

  int next_entry = (GD->ticks + 1) % LENGTHOF(GD->player.damage_history);
  int curr_entry = (GD->ticks) % LENGTHOF(GD->player.damage_history);
  GD->player.dps -= GD->player.damage_history[next_entry];
  GD->player.damage_history[next_entry] = 0;
}

void SpawnNewProjs(GameData* GD) {
  // spawn new projs
  GD->player.reload_progress += fixed_factor * fixed_factor / target_fps / GD->player.reload_delay;
  while (GD->player.reload_progress >= fixed_factor) {
    GD->player.reload_progress -= fixed_factor;
    GD->player.shot_progress += GD->player.shot_count;
  }
  bool first_shot_in_volley = true;
  while (GD->player.shot_progress >= fixed_new(1, 0)) {
    GD->player.shot_progress -= fixed_new(1, 0);
    for (int p = 0; p < LENGTHOF(GD->projs); ++p) {
      if (GD->projs[p].exists) {
        continue;
      }
      GD->projs[p].exists = true;
      GD->projs[p].x = GD->player.x;
      GD->projs[p].y = GD->player.y;
      if (first_shot_in_volley) {
        GD->projs[p].move_speed = GD->player.shot_speed;
        GD->projs[p].angle = GD->player.angle + GetRandomValue(-GD->player.shot_spread, GD->player.shot_spread);
        GD->projs[p].size = 6 + (GD->player.damage / 40);
        GD->projs[p].damage = GD->player.damage;
        GD->projs[p].despawn_timer = 120;
        GD->projs[p].kb = GD->player.shot_kb;
        first_shot_in_volley = false;
      } else {
        GD->projs[p].move_speed = GD->player.shot_speed * GetRandomValue(50, 90) / 100;
        GD->projs[p].angle = GD->player.angle + GetRandomValue(-GD->player.shot_spread * 2, GD->player.shot_spread * 2);
        GD->projs[p].size = 4 + (GD->player.damage / 40);
        GD->projs[p].damage = GD->player.damage;
        GD->projs[p].despawn_timer = 120;
        GD->projs[p].kb = GD->player.shot_kb / 2;
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
      GD->projs[p].exists = false;
      continue;
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
      if (sqdist < int_sq(GD->shapes[s].size + GD->projs[p].size)) {
        GD->projs[p].despawn_timer = 0;
        GD->shapes[s].hp -= GD->projs[p].damage;
        GD->player.dps += GD->projs[p].damage;
        GD->player.damage_history[GD->ticks % LENGTHOF(GD->player.damage_history)] += GD->projs[p].damage;
        if (GD->shapes[s].hp <= 0 && GetRandomValue(0, (GD->pickups_spawned <= 5 ? 1 : 5)) == 0) {
          GD->shapes[s].spawn_pickup_on_despawn = true;
          ++GD->pickups_spawned;
        }
        GD->shapes[s].ticks_since_damaged = 0;
        GD->shapes[s].kb_angle = GD->projs[p].angle;
        GD->shapes[s].kb_speed = GD->projs[p].kb * GD->shapes[s].max_move_speed / fixed_factor;
      }
    }
  }
}

void UpdatePickups(GameData* GD) {
  for (int p = 0; p < LENGTHOF(GD->pickups); ++p) {
    if (!GD->pickups[p].exists) {
      continue;
    }
    // ...
  }
}