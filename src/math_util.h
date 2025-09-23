#pragma once

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

typedef int32_t fixed_t;
typedef uint8_t angle_t;  // [0, TAU)

extern const int fixed_bits;
extern const int fixed_factor;
extern const int angle_factor;

fixed_t FixNew(int32_t whole, uint8_t part);
int32_t FixWhole(fixed_t fix);
uint8_t FixPart(fixed_t fix);

fixed_t FixAbs(fixed_t fix);

fixed_t FixSin(angle_t ang);
fixed_t FixCos(angle_t ang);

void IntClamp(int* val, int min, int max);
void FixClamp(fixed_t* val, fixed_t min, fixed_t max);

int IntSq(int x);
fixed_t FixSq(fixed_t fix);

void IntNudge(int* num, int target, int delta);
void FixNudge(fixed_t* fix, fixed_t target, fixed_t delta);
void FixLerp(fixed_t* val, fixed_t target, fixed_t n);
void FixMove(fixed_t* x, fixed_t* y, fixed_t dist, angle_t ang);

angle_t AngFromSlope(fixed_t dx, fixed_t dy);
angle_t AngFromLine(fixed_t ax, fixed_t ay, fixed_t bx, fixed_t by);
int AngDiff(angle_t src, angle_t dst);
void AngRotateTowards(angle_t* ang, angle_t target, int turn_speed);

int IntMin(int a, int b);
fixed_t FixMin(fixed_t a, fixed_t b);
int IntMax(int a, int b);
fixed_t FixMax(fixed_t a, fixed_t b);