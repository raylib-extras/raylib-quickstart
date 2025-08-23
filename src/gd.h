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

extern const fixed_t default_z;  // z is distance from camera

typedef struct PlayerStats {
  int damage;
  fixed_t max_move_speed;
  fixed_t reload_delay;
  fixed_t shot_count;
  fixed_t shot_kb;
  int shot_pierce;
  fixed_t shot_speed;
  int shot_spread;
  int sight_range;
  int size;
  int turn_speed;

  int magnetism_dist;
  int shot_homing_power;
  int view_distance;
} PlayerStats;

typedef struct Player {
  fixed_t x;
  fixed_t y;
  fixed_t move_speed;
  angle_t angle;

  PlayerStats prev_stats;
  PlayerStats stats;
  int stat_update_timer;

  fixed_t reload_progress;
  fixed_t shot_progress;

  ItemType item_counts[ITEM_COUNT];

  int dps;
  int damage_history[60];
} Player;

typedef enum ShapeVariant {
  SHAPE_VARIANT_NONE,
  SHAPE_VARIANT_BIG,
  SHAPE_VARIANT_FAST,
  SHAPE_VARIANT_HEALING,
  SHAPE_VARIANT_COUNT,
} ShapeVariant;

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
  ShapeVariant variant;

  fixed_t x;
  fixed_t y;
  fixed_t max_move_speed;
  fixed_t move_speed;
  angle_t move_angle;

  fixed_t kb_speed;
  angle_t kb_angle;

  int sqdist_to_player;
  angle_t angle_to_player;

  bool always_target;
  fixed_t target_x;
  fixed_t target_y;
  int sqdist_to_target;
  angle_t angle_to_target;

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

  bool is_homing;
  int homing_max_dist;
  int homing_power;

  uint8_t hit_shape_ids[16];
} Proj;

typedef struct Pickup {
  bool exists;

  fixed_t x;
  fixed_t y;
  ItemType type;

  int sqdist_to_player;
  angle_t angle_to_player;
} Pickup;

typedef struct TextFx {
  bool exists;

  fixed_t x;
  fixed_t y;
  int despawn_timer;
  char text[8];
} TextFx;

typedef struct GdCamera {  // raylib already defines struct Camera >:(
  fixed_t x;
  fixed_t y;
  fixed_t zoom;
} GdCamera;

typedef struct GameData {
  Player player;

  GdCamera camera;

  Shape shapes[80];
  int shape_count;

  Proj projs[40];

  Pickup pickups[40];
  int pickups_spawned;

  TextFx text_fx[40];

  int ticks;
  Font font;

} GameData;