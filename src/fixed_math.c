#include "fixed_math.h"

#include <limits.h>
#include <math.h>
#include <stdint.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int fixed_bits = 8;
const int fixed_factor = 1 << fixed_bits;
const int angle_factor = 1 << (CHAR_BIT * sizeof(angle_t));

// clang-format off
const fixed_t sin_table[320] = {0,6,12,18,25,31,37,43,49,56,62,68,74,80,86,92,97,103,109,115,120,126,131,136,142,147,152,157,162,167,171,176,181,185,189,193,197,201,205,209,212,216,219,222,225,228,231,234,236,238,241,243,244,246,248,249,251,252,253,254,254,255,255,255,255,255,255,255,254,254,253,252,251,249,248,246,244,243,241,238,236,234,231,228,225,222,219,216,212,209,205,201,197,193,189,185,181,176,171,167,162,157,152,147,142,136,131,126,120,115,109,103,97,92,86,80,74,68,62,56,49,43,37,31,25,18,12,6,0,-6,-12,-18,-25,-31,-37,-43,-49,-56,-62,-68,-74,-80,-86,-92,-97,-103,-109,-115,-120,-126,-131,-136,-142,-147,-152,-157,-162,-167,-171,-176,-181,-185,-189,-193,-197,-201,-205,-209,-212,-216,-219,-222,-225,-228,-231,-234,-236,-238,-241,-243,-244,-246,-248,-249,-251,-252,-253,-254,-254,-255,-255,-255,-255,-255,-255,-255,-254,-254,-253,-252,-251,-249,-248,-246,-244,-243,-241,-238,-236,-234,-231,-228,-225,-222,-219,-216,-212,-209,-205,-201,-197,-193,-189,-185,-181,-176,-171,-167,-162,-157,-152,-147,-142,-136,-131,-126,-120,-115,-109,-103,-97,-92,-86,-80,-74,-68,-62,-56,-49,-43,-37,-31,-25,-18,-12,-6,0,6,12,18,25,31,37,43,49,56,62,68,74,80,86,92,97,103,109,115,120,126,131,136,142,147,152,157,162,167,171,176,181,185,189,193,197,201,205,209,212,216,219,222,225,228,231,234,236,238,241,243,244,246,248,249,251,252,253,254,254,255,255,255};
// clang-format on

fixed_t fixed_new(int32_t whole, uint8_t part) { return (whole << fixed_bits) | part; }
int32_t fixed_whole(fixed_t fix) { return fix >> fixed_bits; };
uint8_t fixed_part(fixed_t fix) { return fix & ((1 << fixed_bits) - 1); }
fixed_t fixed_abs(fixed_t fix) {
  if (fix < 0) {
    return -fix;
  }
  return fix;
};
fixed_t fixed_sin(angle_t ang) { return sin_table[ang]; }
fixed_t fixed_cos(angle_t ang) { return sin_table[ang + (angle_factor / 4)]; }

void clamp(int* val, int min, int max) {
  if (*val < min) {
    *val = min;
    return;
  }
  if (*val > max) {
    *val = max;
  }
}

void fixed_clamp(fixed_t* val, fixed_t min, fixed_t max) {
  if (*val < min) {
    *val = min;
    return;
  }
  if (*val > max) {
    *val = max;
  }
}

int int_sq(int x) { return x * x; }
fixed_t fixed_sq(fixed_t fix) {
  // ((fix / fixed_factor) * (fix / fixed_factor)) * fixed_factor
  // but with more precision for smaller fixed numbers.
  return fix * fix / fixed_factor;
}

void nudge(int* num, int target, int delta) {
  if (*num + target < delta) {
    *num += delta;
  } else if (*num - target < delta) {
    *num -= delta;
  } else {
    *num = target;
  }
}

void fixed_nudge(fixed_t* fix, fixed_t target, fixed_t delta) {
  if (*fix + delta < target) {
    *fix += delta;
  } else if (*fix - delta > target) {
    *fix -= delta;
  } else {
    *fix = target;
  }
}

fixed_t fixed_lerp(fixed_t a, fixed_t b, fixed_t n) { return a + (((b - a) * n) >> fixed_bits); }

angle_t angle_from_slope(fixed_t dx, fixed_t dy) { return atan2(dy, dx) * angle_factor / (2 * M_PI); }

angle_t angle_from_line(fixed_t ax, fixed_t ay, fixed_t bx, fixed_t by) {
  fixed_t dy = by - ay;
  fixed_t dx = bx - ax;
  return angle_from_slope(dx, dy);
}

int angle_diff(angle_t src, angle_t dst) {
  return (int8_t)((int)dst - (int)src);
}

void angle_rotate_towards(angle_t* ang, angle_t target, int turn_speed) {
  int diff = angle_diff(*ang, target);
  if (diff > turn_speed) {
    *ang += turn_speed;
  } else if (diff < -turn_speed) {
    *ang -= turn_speed;
  } else {
    *ang += diff;
  }
}
