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
      shape->sides = 3;
      shape->size = 8;
      shape->fg = YELLOW;
      shape->bg = GOLD;
    } break;
    case 4: {
      shape->max_hp = 40;
      shape->sides = 4;
      shape->size = 12;
      shape->fg = PINK;
      shape->bg = RED;
    } break;
    case 5: {
      shape->max_hp = 100;
      shape->sides = 5;
      shape->size = 24;
      shape->fg = PURPLE;
      shape->bg = VIOLET;
    } break;
    case 6: {
      shape->max_hp = 500;
      shape->sides = 6;
      shape->size = 8;
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
  }
}

void SpawnNewShapes(GameData* GD) {
  if (GD->ticks % 4 == 0 && GD->shape_count < (LENGTHOF(GD->shapes) / 2)) {
    int i = ClaimEmptyShapeSlot(GD);
    if (i == -1) {
      return;
    }
    GD->shapes[i].x = GD->player.x + fixed_new(GetRandomValue(-render_w, render_w), 0);
    GD->shapes[i].y = GD->player.y + fixed_new(GetRandomValue(-render_h, render_h), 0);
    GD->shapes[i].move_speed = fixed_new(GetRandomValue(1, 8), 0) / target_fps;
    GD->shapes[i].angle = GetRandomValue(0, angle_factor - 1);
    int rand = GetRandomValue(0, 9);
    if (rand < 7) {
      SetShapeStats(&GD->shapes[i], 3);
    } else if (rand < 9) {
      SetShapeStats(&GD->shapes[i], 4);
    } else {
      SetShapeStats(&GD->shapes[i], 5);
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

void UpdateShapes(GameData* GD) {
  // shape logic
  for (int i = 0; i < LENGTHOF(GD->shapes); ++i) {
    if (!GD->shapes[i].exists) {
      continue;
    }

    // hp and damage-related
    if (GD->shapes[i].hp <= 0) {
      GD->shapes[i].marked_for_despawn = true;
    }
    ++GD->shapes[i].ticks_since_damaged;
    if (GD->shapes[i].ticks_since_damaged > 240 && GD->ticks % 30 == 0 && GD->shapes[i].hp < GD->shapes[i].max_hp) {
      ++GD->shapes[i].hp;
    }
    if (GD->shapes[i].i_frames > 0) {
      --GD->shapes[i].i_frames;
    }

    // despawn timer
    if (GD->shapes[i].marked_for_despawn) {
      // printf("Despawned shape %d\n", i);
      if (GD->shapes[i].sides > 3) {
        SpawnChildShapes(GD, i);
      }
      GD->shapes[i].exists = false;
      --GD->shape_count;
      continue;
    }

    // movement
    GD->shapes[i].x += fixed_cos(GD->shapes[i].angle) * GD->shapes[i].move_speed / fixed_factor;
    GD->shapes[i].y += fixed_sin(GD->shapes[i].angle) * GD->shapes[i].move_speed / fixed_factor;
    if (GD->shapes[i].move_speed > fixed_new(8, 0) / target_fps) {
      GD->shapes[i].move_speed -= 8;
    }

    // update sqdist_to_player
    {
      int dx = abs(fixed_whole(GD->shapes[i].x) - fixed_whole(GD->player.x));
      int dy = abs(fixed_whole(GD->shapes[i].y) - fixed_whole(GD->player.y));
      GD->shapes[i].sqdist_to_player = int_sq(dx) + int_sq(dy);
    }
    // player distance-related despawning
    if (GD->shapes[i].sqdist_to_player > int_sq(render_h)) {
      GD->shapes[i].marked_for_despawn = true;
    }

    if (GD->ticks % 10 == 0) {
      // shape-shape collision
      for (int j = i + 1; j < LENGTHOF(GD->shapes); ++j) {
        if (!GD->shapes[j].exists) {
          continue;
        }
        int dx = abs(fixed_whole(GD->shapes[i].x) - fixed_whole(GD->shapes[j].x));
        int dy = abs(fixed_whole(GD->shapes[i].y) - fixed_whole(GD->shapes[j].y));
        int sqdist = int_sq(dx) + int_sq(dy);
        if (sqdist < int_sq((GD->shapes[i].size + GD->shapes[j].size) / 2)) {
          if (GD->shapes[i].x < GD->shapes[j].x) {
            GD->shapes[i].x -= fixed_new(1, 0);
            GD->shapes[j].x += fixed_new(1, 0);
          } else {
            GD->shapes[i].x += fixed_new(1, 0);
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
  for (int i = 0; i < LENGTHOF(GD->shapes); ++i) {
    if (GD->shapes[i].exists) {
      printf("%c", GD->shapes[i].sides + '0');

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
  GD->player.move_speed = fixed_new(20, 0) / GD->player.size;
  if (IsKeyDown(KEY_A)) GD->player.x -= GD->player.move_speed;
  if (IsKeyDown(KEY_D)) GD->player.x += GD->player.move_speed;
  if (IsKeyDown(KEY_S)) GD->player.y += GD->player.move_speed;
  if (IsKeyDown(KEY_W)) GD->player.y -= GD->player.move_speed;

  // find index of closest shape
  int closest_sqdist = INT32_MAX;
  int closest_shape_idx = -1;
  for (int i = 0; i < LENGTHOF(GD->shapes); ++i) {
    if (GD->shapes[i].exists) {
      if (GD->shapes[i].sqdist_to_player < int_sq(GD->player.sight_range) &&
          GD->shapes[i].sqdist_to_player < closest_sqdist) {
        closest_shape_idx = i;
        closest_sqdist = GD->shapes[i].sqdist_to_player;
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
}

void SpawnNewProjs(GameData* GD) {
  // spawn new projs
  if (GD->ticks % 15 == 0) {
    for (int i = 0; i < LENGTHOF(GD->projs); ++i) {
      if (GD->projs[i].exists) {
        continue;
      }
      GD->projs[i].exists = true;
      GD->projs[i].x = GD->player.x;
      GD->projs[i].y = GD->player.y;
      GD->projs[i].move_speed = fixed_new(240, 0) / target_fps;
      GD->projs[i].angle = GD->player.angle;
      GD->projs[i].damage = 4;
      GD->projs[i].despawn_timer = 120;
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
      if (sqdist < int_sq(GD->shapes[s].size)) {
        GD->projs[p].despawn_timer = 0;
        GD->shapes[s].hp -= GD->projs[p].damage;
        GD->shapes[s].ticks_since_damaged = 0;
        GD->shapes[s].angle = GD->projs[p].angle;
        GD->shapes[s].move_speed = fixed_new(32, 0) / target_fps;
      }
    }
  }
}