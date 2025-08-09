#pragma once

#include <raylib.h>

#include "fixed_math.h"
#include "gd.h"

void SpawnNewShapes(GameData* GD);
void UpdateShapes(GameData* GD);

void UpdatePlayer(GameData* GD);

void SpawnNewProjs(GameData* GD);
void UpdateProjs(GameData* GD);