#pragma once

#include "fixed_math.h"
#include "gd.h"

int GetRenderLength(GameData* GD, int length, fixed_t z);
void GetRenderCoords(GameData* GD, fixed_t x, fixed_t y, fixed_t z, int* ox, int* oy);