#pragma once

#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

#include "fixed_math.h"
#include "item_types.h"

#define LENGTHOF(arr) (sizeof((arr)) / sizeof((arr)[0]))

#ifdef NDEBUG
#define DrawPrintf(x, y, color, text, ...)
#else
#define DrawPrintf(x, y, color, text, ...)                                               \
  do {                                                                                   \
    char out[256];                                                                       \
    if (snprintf(out, LENGTHOF(out), text __VA_OPT__(, ) __VA_ARGS__) > LENGTHOF(out)) { \
      TraceLog(LOG_WARNING, "DrawPrintf output truncated");                              \
    }                                                                                    \
    DrawTextEx(GD->font, out, (Vector2){(float)x, (float)y}, 8, 0, color);               \
  } while (0)
#endif

extern const int render_w;
extern const int render_h;
extern const int window_scale;
extern const int window_w;
extern const int window_h;

extern const int target_fps;

typedef struct Player {
  fixed_t x;
  fixed_t y;
  fixed_t move_speed;
  angle_t angle;

  int size;
  fixed_t max_move_speed;
  int sight_range;
  int turn_speed;
  fixed_t reload_delay;
  fixed_t reload_progress;
  int damage;
  int shot_spread;
  fixed_t shot_speed;
  fixed_t shot_count;
  fixed_t shot_progress;

  fixed_t shot_kb;
  int shot_pierce;

  ItemType item_counts[ITEM_COUNT];

  int dps;
  int damage_history[60];
} Player;

typedef struct Shape {
  bool exists;
  bool marked_for_despawn;
  bool spawn_pickup_on_despawn;
  bool spawn_children_on_despawn;

  uint8_t id;

  int sides;
  int size;
  Color fg;
  Color bg;

  fixed_t x;
  fixed_t y;
  fixed_t max_move_speed;
  fixed_t move_speed;
  angle_t move_angle;

  fixed_t kb_speed;
  angle_t kb_angle;

  int sqdist_to_player;
  angle_t angle_to_player;

  int hp;
  int max_hp;
  int regen;
  int ticks_since_damaged;
  int i_frames;
} Shape;

typedef struct Proj {
  bool exists;

  fixed_t x;
  fixed_t y;
  fixed_t move_speed;
  angle_t angle;

  int damage;
  int pierce;
  int size;
  int despawn_timer;
  fixed_t kb;

  uint8_t hit_shape_ids[16];
} Proj;

typedef struct Pickup {
  bool exists;

  fixed_t x;
  fixed_t y;
  ItemType type;
} Pickup;

typedef struct TextFx {
  bool exists;

  fixed_t x;
  fixed_t y;
  int despawn_timer;
  char text[8];
} TextFx;

typedef struct GameData {
  Player player;

  struct {
    fixed_t x;
    fixed_t y;
  } camera;

  Shape shapes[80];
  int shape_count;

  Proj projs[40];

  Pickup pickups[40];
  int pickups_spawned;

  TextFx text_fx[10];

  int ticks;
  Font font;

} GameData;