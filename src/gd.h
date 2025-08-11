#pragma once

#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

#include "fixed_math.h"

#define LENGTHOF(arr) (sizeof((arr)) / sizeof((arr)[0]))

#ifdef NDEBUG
#define DrawPrintf(x, y, color, text, ...)
#else
#define DrawPrintf(x, y, color, text, ...)                                 \
  do {                                                                     \
    char out[100];                                                         \
    snprintf(out, LENGTHOF(out), text __VA_OPT__(, ) __VA_ARGS__);         \
    DrawTextEx(GD->font, out, (Vector2){(float)x, (float)y}, 8, 0, color); \
  } while (0)
#endif

extern const int render_w;
extern const int render_h;
extern const int window_scale;
extern const int window_w;
extern const int window_h;

extern const int target_fps;

typedef struct Shape {
  bool exists;

  fixed_t x;
  fixed_t y;
  fixed_t move_speed;
  angle_t angle;

  int hp;
  int max_hp;
  int regen;
  int ticks_since_damaged;
  int sqdist_to_player;
  int sides;
  int size;
  Color fg;
  Color bg;
  bool marked_for_despawn;
  bool spawn_pickup_on_despawn;
  int i_frames;
} Shape;

typedef struct Proj {
  bool exists;

  fixed_t x;
  fixed_t y;
  fixed_t move_speed;
  angle_t angle;

  int damage;
  int size;
  int despawn_timer;
} Proj;

typedef enum ItemType {
  ITEM_SPEED_UPGRADE,
  ITEM_FIRE_RATE_UPGRADE,
  ITEM_TURN_SPEED_UPGRADE,
  ITEM_DAMAGE_UPGRADE,
  ITEM_COUNT
} ItemType;

typedef struct Pickup {
  bool exists;

  fixed_t x;
  fixed_t y;
  ItemType type;
} Pickup;

typedef struct GameData {
  struct {
    fixed_t x;
    fixed_t y;
    fixed_t move_speed;
    angle_t angle;

    int size;
    fixed_t max_speed;
    int sight_range;
    int turn_speed;
    fixed_t reload_delay;
    fixed_t reload_progress;
    int damage;
    int shot_spread;

    ItemType item_counts[ITEM_COUNT];
  } player;

  struct {
    fixed_t x;
    fixed_t y;
  } camera;

  Shape shapes[40];
  int shape_count;

  Proj projs[40];

  Pickup pickups[40];
  int pickups_spawned;

  int ticks;
  Font font;

} GameData;