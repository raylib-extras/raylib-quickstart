#pragma once

#include "gd.h"
#include "math_util.h"

int GetRenderLength(GameScene* GS, int length, fixed_t z);
void GetRenderCoords(GameScene* GS, fixed_t x, fixed_t y, fixed_t z, int* ox, int* oy);