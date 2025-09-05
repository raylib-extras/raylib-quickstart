#pragma once

#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

#include "fixed_math.h"
#include "gs.h"
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
    DrawTextEx(GS->font, out, (Vector2){(float)x, (float)y}, 8, 0, color);               \
  } while (0)
#endif

extern const int render_w;
extern const int render_h;
extern int window_scale;
extern int window_w;
extern int window_h;

extern const int target_fps;

extern const fixed_t default_z;  // z is distance from camera

typedef struct GameData {
  int unused;
  GameScene GS;

} GameData;