#pragma once

#include <raylib.h>

#include "fixed_math.h"
#include "gd.h"

void InitGameData(GameData* GD);

void SpawnNewShapes(GameData* GD);
void UpdateShapes(GameData* GD);

void UpdatePlayer(GameData* GD);

void SpawnNewProjs(GameData* GD);
void UpdateProjs(GameData* GD);

void UpdatePickups(GameData* GD);
