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
      shape->max_hp = 20;
      shape->regen = 0;
      shape->sides = 3;
      shape->size = 8;
      shape->fg = YELLOW;
      shape->bg = GOLD;
    } break;
    case 4: {
      shape->max_hp = 40;
      shape->regen = 4;
      shape->sides = 4;
      shape->size = 12;
      shape->fg = PINK;
      shape->bg = RED;
    } break;
    case 5: {
      shape->max_hp = 100;
      shape->regen = 12;
      shape->sides = 5;
      shape->size = 24;
      shape->fg = PURPLE;
      shape->bg = VIOLET;
    } break;
    case 6: {
      shape->max_hp = 500;
      shape->regen = 50;
      shape->sides = 6;
      shape->size = 40;
      shape->fg = GREEN;
      shape->bg = LIME;
    } break;
    default: {
      TraceLog(LOG_ERROR, "Bad Shape side count of %d", sides);
    } break;
  }
  shape->hp = shape->max_hp;
  shape->marked_for_despawn = 0;
}

void SpawnChildShapes(GameData* GD, int parent) {
  int sides = GD->shapes[parent].sides;
  angle_t ang = GetRandomValue(0, angle_factor - 1);
  for (int i = 0; i < sides; ++i) {
    ang += (angle_factor / sides);
    int s = ClaimEmptyShapeSlot(GD);
    if (s == -1) {
      return;
    }
    GD->shapes[s].x = GD->shapes[parent].x;
    GD->shapes[s].y = GD->shapes[parent].y;
    GD->shapes[s].move_speed = fixed_new(90, 0) / target_fps;
    GD->shapes[s].angle = ang;
    GD->shapes[s].i_frames = 30;
    SetShapeStats(&GD->shapes[s], sides - 1);
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
    return;
  }
}

void UpdatePlayerStats(GameData* GD) {
  GD->player.size = 15;
  GD->player.max_speed = fixed_new(1, 128);
  GD->player.reload_delay = fixed_new(0, 128);
  GD->player.sight_range = render_h / 2;
  GD->player.turn_speed = 2;
  GD->player.damage = 4;
  GD->player.shot_spread = 1;
  for (int i = 0; i < ITEM_COUNT; ++i) {
    for (int x = 0; x < GD->player.item_counts[i]; ++x) {
      switch (i) {
        case ITEM_SPEED_UPGRADE:
          GD->player.max_speed += fixed_new(0, 128);
          break;
        case ITEM_FIRE_RATE_UPGRADE:
          GD->player.reload_delay = GD->player.reload_delay * 80 / 100;
          GD->player.shot_spread += 2;
          break;
        case ITEM_TURN_SPEED_UPGRADE:
          ++GD->player.turn_speed;
          break;
        case ITEM_DAMAGE_UPGRADE:
          GD->player.damage += 2;
          GD->player.reload_delay = GD->player.reload_delay * 105 / 100;
          break;
      }
    }
  }

  clamp(&GD->player.size, 10, 50);
  fixed_clamp(&GD->player.max_speed, fixed_new(1, 0), fixed_new(5, 0));
  fixed_clamp(&GD->player.reload_delay, fixed_new(0, 8), fixed_new(5, 0));
  // clamp(&GD->player.sight_range);
  clamp(&GD->player.turn_speed, 1, 64);
  clamp(&GD->player.damage, 1, 999);
  clamp(&GD->player.shot_spread, 0, 32);
}

void SpawnNewShapes(GameData* GD) {
  if (GD->ticks % 30 == 0 && GD->shape_count < (LENGTHOF(GD->shapes) / 2)) {
    int i = ClaimEmptyShapeSlot(GD);
    if (i == -1) {
      return;
    }
    GD->shapes[i].x = GD->player.x + fixed_new(GetRandomValue(-render_w, render_w), 0);
    GD->shapes[i].y = GD->player.y + fixed_new(GetRandomValue(-render_h, render_h), 0);
    GD->shapes[i].move_speed = fixed_new(GetRandomValue(1, 8), 0) / target_fps;
    GD->shapes[i].angle = GetRandomValue(0, angle_factor - 1);
    int rand = GetRandomValue(0, 99);
    if (rand < 70) {
      SetShapeStats(&GD->shapes[i], 3);
    } else if (rand < 85) {
      SetShapeStats(&GD->shapes[i], 4);
    } else if (rand < 95) {
      SetShapeStats(&GD->shapes[i], 5);
    } else {
      SetShapeStats(&GD->shapes[i], 6);
    }
    // printf("Spawned shape %d\n", i);
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
    if (GD->shapes[s].ticks_since_damaged > 240 && GD->ticks % 30 == 0 && GD->shapes[s].hp < GD->shapes[s].max_hp) {
      ++GD->shapes[s].hp;
    }
    if (GD->shapes[s].i_frames > 0) {
      --GD->shapes[s].i_frames;
    }
    if (GD->ticks % 60 < GD->shapes[s].regen && (GD->shapes[s].hp < GD->shapes[s].max_hp)) {
      GD->shapes[s].hp += 1;
    }

    // despawn timer
    if (GD->shapes[s].marked_for_despawn) {
      // printf("Despawned shape %d\n", i);
      if (GD->shapes[s].sides > 3) {
        SpawnChildShapes(GD, s);
      } else {
        if (GD->shapes[s].spawn_pickup_on_despawn) {
          SpawnPickup(GD, s);
        }
      }
      GD->shapes[s].exists = false;
      --GD->shape_count;
      continue;
    }

    // movement
    GD->shapes[s].x += fixed_cos(GD->shapes[s].angle) * GD->shapes[s].move_speed / fixed_factor;
    GD->shapes[s].y += fixed_sin(GD->shapes[s].angle) * GD->shapes[s].move_speed / fixed_factor;
    if (GD->shapes[s].move_speed > fixed_new(8, 0) / target_fps) {
      GD->shapes[s].move_speed -= 8;
    }

    // update sqdist_to_player
    {
      int dx = abs(fixed_whole(GD->shapes[s].x) - fixed_whole(GD->player.x));
      int dy = abs(fixed_whole(GD->shapes[s].y) - fixed_whole(GD->player.y));
      GD->shapes[s].sqdist_to_player = int_sq(dx) + int_sq(dy);
    }
    // player distance-related despawning
    if (GD->shapes[s].sqdist_to_player > int_sq(render_h)) {
      GD->shapes[s].marked_for_despawn = true;
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
  for (int s = 0; s < LENGTHOF(GD->shapes); ++s) {
    if (GD->shapes[s].exists) {
      printf("%c", GD->shapes[s].sides + '0');

    } else {
      printf("_");
    }
  }
  printf(" %d\n", GD->shape_count);
}

void UpdatePlayer(GameData* GD) {
  // player shrinking
  if (GD->ticks % 120 == 0 && GD->player.size > 10) {
    --GD->player.size;
  }
  if (GD->ticks % 15 == 0 && GD->player.size > 40) {
    --GD->player.size;
  }

  // player movement
  GD->player.move_speed = GD->player.max_speed;
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
    int diff = angle_diff(GD->player.angle, target_angle);
    if (diff > GD->player.turn_speed)
      GD->player.angle += GD->player.turn_speed;
    else if (diff < -GD->player.turn_speed)
      GD->player.angle -= GD->player.turn_speed;
    else
      GD->player.angle += diff;
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
}

void SpawnNewProjs(GameData* GD) {
  // spawn new projs
  GD->player.reload_progress += fixed_factor * fixed_factor / target_fps / GD->player.reload_delay;
  while (GD->player.reload_progress >= fixed_factor) {
    GD->player.reload_progress -= fixed_factor;
    for (int p = 0; p < LENGTHOF(GD->projs); ++p) {
      if (GD->projs[p].exists) {
        continue;
      }
      GD->projs[p].exists = true;
      GD->projs[p].x = GD->player.x;
      GD->projs[p].y = GD->player.y;
      GD->projs[p].move_speed = fixed_new(240, 0) / target_fps;
      GD->projs[p].angle = GD->player.angle + GetRandomValue(-GD->player.shot_spread, GD->player.shot_spread);
      GD->projs[p].size = 4 + (GD->player.damage / 3);
      GD->projs[p].damage = GD->player.damage;
      GD->projs[p].despawn_timer = 120;
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
        if (GD->shapes[s].hp <= 0 && GetRandomValue(0, (GD->pickups_spawned <= 7 ?: 7)) == 0) {
          GD->shapes[s].spawn_pickup_on_despawn = true;
          ++GD->pickups_spawned;
        }
        GD->shapes[s].ticks_since_damaged = 0;
        GD->shapes[s].angle = GD->projs[p].angle;
        GD->shapes[s].move_speed = fixed_new(32, 0) / target_fps;
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