#include "update_logic.h"

void SpawnNewShapes(GameData* GD) {
  if (GD->ticks % 4 == 0) {
    for (int i = 0; i < LENGTHOF(GD->shapes); ++i) {
      if (GD->shapes[i].exists) {
        continue;
      }
      GD->shapes[i].exists = true;
      GD->shapes[i].x = GD->player.x + fixed_new(GetRandomValue(-render_w, render_w), 0);
      GD->shapes[i].y = GD->player.y + fixed_new(GetRandomValue(-render_h, render_h), 0);
      GD->shapes[i].move_speed = fixed_new(GetRandomValue(1, 8), 0) / 60;
      GD->shapes[i].angle = GetRandomValue(0, angle_factor - 1);
      int rand = GetRandomValue(0, 7);
      if (rand < 5) {
        GD->shapes[i].max_hp = 20;
        GD->shapes[i].sides = 3;
        GD->shapes[i].size = 8;
        GD->shapes[i].fg = YELLOW;
        GD->shapes[i].bg = GOLD;
      } else if (rand < 7) {
        GD->shapes[i].max_hp = 40;
        GD->shapes[i].sides = 4;
        GD->shapes[i].size = 12;
        GD->shapes[i].fg = PINK;
        GD->shapes[i].bg = RED;
      } else {
        GD->shapes[i].max_hp = 100;
        GD->shapes[i].sides = 5;
        GD->shapes[i].size = 24;
        GD->shapes[i].fg = PURPLE;
        GD->shapes[i].bg = VIOLET;
      }
      GD->shapes[i].hp = GD->shapes[i].max_hp;
      GD->shapes[i].despawn_timer = GetRandomValue(1200, 1800);
      // printf("Spawned shape %d\n", i);
      break;
    }
  }
}

void UpdateShapes(GameData* GD) {
  // shape logic
  for (int i = 0; i < LENGTHOF(GD->shapes); ++i) {
    if (!GD->shapes[i].exists) {
      continue;
    }
    // hp and damage-related
    if (GD->shapes[i].hp <= 0) {
      GD->shapes[i].despawn_timer = 0;
    }
    ++GD->shapes[i].ticks_since_damaged;
    if (GD->shapes[i].ticks_since_damaged > 240 && GD->ticks % 30 == 0 && GD->shapes[i].hp < GD->shapes[i].max_hp) {
      ++GD->shapes[i].hp;
    }
    // despawn timer
    --GD->shapes[i].despawn_timer;
    if (GD->shapes[i].despawn_timer <= 0) {
      // printf("Despawned shape %d\n", i);
      GD->shapes[i].exists = false;
      continue;
    }
    // movement
    GD->shapes[i].x += fixed_cos(GD->shapes[i].angle) * GD->shapes[i].move_speed / fixed_factor;
    GD->shapes[i].y += fixed_sin(GD->shapes[i].angle) * GD->shapes[i].move_speed / fixed_factor;
    if (GD->shapes[i].move_speed > fixed_new(8, 0) / 60) {
      GD->shapes[i].move_speed -= 4;
    }
    // update sqdist_to_player
    int dx = abs(fixed_whole(GD->shapes[i].x) - fixed_whole(GD->player.x));
    int dy = abs(fixed_whole(GD->shapes[i].y) - fixed_whole(GD->player.y));
    GD->shapes[i].sqdist_to_player = int_sq(dx) + int_sq(dy);
    // absorption
    if (GD->shapes[i].sqdist_to_player < int_sq(GD->player.size - 4)) {
      GD->player.size += 1;
      GD->shapes[i].hp -= 10;
    } else if (GD->shapes[i].sqdist_to_player < int_sq(GD->player.size + 8)) {
      angle_t target_angle = angle_from_line(GD->shapes[i].x, GD->shapes[i].y, GD->player.x, GD->player.y);
      GD->shapes[i].angle = target_angle;
    }
  }
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
      GD->projs[i].move_speed = fixed_new(120, 0) / 60;
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
      int dx = abs(fixed_whole(GD->shapes[s].x) - fixed_whole(GD->projs[p].x));
      int dy = abs(fixed_whole(GD->shapes[s].y) - fixed_whole(GD->projs[p].y));
      int sqdist = int_sq(dx) + int_sq(dy);
      if (sqdist < int_sq(GD->shapes[s].size)) {
        GD->projs[p].despawn_timer = 0;
        GD->shapes[s].hp -= GD->projs[p].damage;
        GD->shapes[s].ticks_since_damaged = 0;
        GD->shapes[s].angle = GD->projs[p].angle;
        GD->shapes[s].move_speed = fixed_new(32, 0) / 60;
      }
    }
  }
}