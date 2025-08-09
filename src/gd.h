#pragma once

#include <stdbool.h>

#include "fixed_math.h"

#define LENGTHOF(arr) (sizeof((arr)) / sizeof((arr)[0]))

char out[80];
#define DrawPrintf(x, y, font_size, color, text, ...)              \
  do {                                                             \
    snprintf(out, LENGTHOF(out), text __VA_OPT__(, ) __VA_ARGS__); \
    DrawText(out, x, y, font_size, color);                         \
  } while (0)

const int render_w = 320;
const int render_h = 240;
const int window_scale = 3;
const int window_w = render_w * window_scale;
const int window_h = render_h * window_scale;

typedef struct Shape {
  bool exists;
  fixed_t x;
  fixed_t y;
  fixed_t move_speed;
  angle_t angle;
  int sqdist_to_player;
  int sides;
  int despawn_timer;
} Shape;

typedef struct Proj {
  bool exists;
  fixed_t x;
  fixed_t y;
  fixed_t move_speed;
  angle_t angle;
  int despawn_timer;
} Proj;

typedef struct GameData {
  struct {
    fixed_t x;
    fixed_t y;
    fixed_t move_speed;
    angle_t angle;
    int size;
    fixed_t sight_range;
    int turn_speed;
  } player;

  struct {
    fixed_t x;
    fixed_t y;
  } camera;

  Shape shapes[40];
  Proj projs[20];

  int ticks;

} GameData;
