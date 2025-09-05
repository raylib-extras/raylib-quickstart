#pragma once

#include "fixed_math.h"
#include "gd.h"

int GetRenderLength(GameScene* GS, int length, fixed_t z);
void GetRenderCoords(GameScene* GS, fixed_t x, fixed_t y, fixed_t z, int* ox, int* oy);