#pragma once

#include <limits.h>
#include <stdint.h>

typedef int32_t fixed_t;
typedef uint8_t angle_t;  // [0, TAU)

typedef struct fixed_pair {
  fixed_t a, b;
} fixed_pair;

extern const int fixed_bits;
extern const int fixed_factor;
extern const int angle_factor;

fixed_t fixed_new(int32_t whole, uint8_t part);
int32_t fixed_whole(fixed_t fix);
uint8_t fixed_part(fixed_t fix);
fixed_t fixed_abs(fixed_t fix);
int abs(int i);
fixed_t fixed_sin(angle_t ang);
fixed_t fixed_cos(angle_t ang);

void clamp(int* val, int min, int max);
void fixed_clamp(fixed_t* val, fixed_t min, fixed_t max);

int int_sq(int x);
fixed_t fixed_sq(fixed_t fix);

fixed_t fixed_nudge(fixed_t* fix, fixed_t goal);
fixed_t fixed_lerp(fixed_t a, fixed_t b, fixed_t n);

fixed_pair fixed_norm(fixed_t dx, fixed_t dy);

angle_t angle_from_slope(fixed_t dx, fixed_t dy);
angle_t angle_from_line(fixed_t ax, fixed_t ay, fixed_t bx, fixed_t by);
int angle_diff(angle_t src, angle_t dst);