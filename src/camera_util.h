#pragma once

#include "fixed_math.h"
#include "gd.h"

fixed_t GetRenderLength(GameData* GD, fixed_t length, fixed_t z);
void GetRenderCoords(GameData* GD, fixed_t x, fixed_t y, fixed_t z, int* ox, int* oy);