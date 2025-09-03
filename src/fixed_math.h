#pragma once

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

typedef int32_t fixed_t;
typedef uint8_t angle_t;  // [0, TAU)

extern const int fixed_bits;
extern const int fixed_factor;
extern const int angle_factor;

fixed_t fixed_new(int32_t whole, uint8_t part);
int32_t fixed_whole(fixed_t fix);
uint8_t fixed_part(fixed_t fix);

fixed_t fixed_abs(fixed_t fix);

fixed_t fixed_sin(angle_t ang);
fixed_t fixed_cos(angle_t ang);

void clamp(int* val, int min, int max);
void fixed_clamp(fixed_t* val, fixed_t min, fixed_t max);

int int_sq(int x);
fixed_t fixed_sq(fixed_t fix);

void nudge(int* num, int target, int delta);
void fixed_nudge(fixed_t* fix, fixed_t target, fixed_t delta);
fixed_t fixed_lerp(fixed_t a, fixed_t b, fixed_t n);

angle_t angle_from_slope(fixed_t dx, fixed_t dy);
angle_t angle_from_line(fixed_t ax, fixed_t ay, fixed_t bx, fixed_t by);
int angle_diff(angle_t src, angle_t dst);
void angle_rotate_towards(angle_t* ang, angle_t target, int turn_speed);

int min(int a, int b);
fixed_t fixed_min(fixed_t a, fixed_t b);
int max(int a, int b);
fixed_t fixed_max(fixed_t a, fixed_t b);