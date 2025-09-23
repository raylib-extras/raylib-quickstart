#include "math_util.h"

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

fixed_t FixNew(int32_t whole, uint8_t part) { return (whole << fixed_bits) | part; }
int32_t FixWhole(fixed_t fix) { return fix >> fixed_bits; };
uint8_t FixPart(fixed_t fix) { return fix & ((1 << fixed_bits) - 1); }
fixed_t FixAbs(fixed_t fix) {
  if (fix < 0) {
    return -fix;
  }
  return fix;
};
fixed_t FixSin(angle_t ang) { return sin_table[ang]; }
fixed_t FixCos(angle_t ang) { return sin_table[ang + (angle_factor / 4)]; }

void IntClamp(int* val, int min, int max) {
  if (*val < min) {
    *val = min;
    return;
  }
  if (*val > max) {
    *val = max;
  }
}

void FixClamp(fixed_t* val, fixed_t min, fixed_t max) {
  if (*val < min) {
    *val = min;
    return;
  }
  if (*val > max) {
    *val = max;
  }
}

int IntSq(int x) { return x * x; }
fixed_t FixSq(fixed_t fix) {
  // ((fix / fixed_factor) * (fix / fixed_factor)) * fixed_factor
  // but with more precision for smaller fixed numbers.
  return fix * fix / fixed_factor;
}

void IntNudge(int* num, int target, int delta) {
  if (*num + delta < target) {
    *num += delta;
  } else if (*num - delta > target) {
    *num -= delta;
  } else {
    *num = target;
  }
}

void FixNudge(fixed_t* fix, fixed_t target, fixed_t delta) {
  if (*fix + delta < target) {
    *fix += delta;
  } else if (*fix - delta > target) {
    *fix -= delta;
  } else {
    *fix = target;
  }
}

void FixLerp(fixed_t* val, fixed_t target, fixed_t n) { *val = *val + (((target - *val) * n) >> fixed_bits); }

void FixMove(fixed_t* x, fixed_t* y, fixed_t dist, angle_t ang) {
  *x += (FixCos(ang) * dist) / fixed_factor;
  *y += (FixSin(ang) * dist) / fixed_factor;
}

angle_t AngFromSlope(fixed_t dx, fixed_t dy) { return atan2(dy, dx) * angle_factor / (2 * M_PI); }

angle_t AngFromLine(fixed_t ax, fixed_t ay, fixed_t bx, fixed_t by) {
  fixed_t dy = by - ay;
  fixed_t dx = bx - ax;
  return AngFromSlope(dx, dy);
}

int AngDiff(angle_t src, angle_t dst) {
  return (int8_t)((int)dst - (int)src);
}

void AngRotateTowards(angle_t* ang, angle_t target, int turn_speed) {
  int diff = AngDiff(*ang, target);
  if (diff > turn_speed) {
    *ang += turn_speed;
  } else if (diff < -turn_speed) {
    *ang -= turn_speed;
  } else {
    *ang += diff;
  }
}

int IntMin(int a, int b) {
  return a < b ? a : b;
}
fixed_t FixMin(fixed_t a, fixed_t b) {
  return a < b ? a : b;
}

int IntMax(int a, int b) {
  return a > b ? a : b;
}
fixed_t FixMax(fixed_t a, fixed_t b) {
  return a > b ? a : b;
}
