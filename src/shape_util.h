#pragma once

#include "gd.h"

void SpawnNewShapes(GameData* GD);
int CompareShapes(const void* p, const void* q);
int ClaimEmptyShapeSlot(GameData* GD);
int PickShapeSides(GameData* GD);
ShapeVariant PickShapeVariant(GameData* GD, int sides);
void SetShapeStats(Shape* shape, int sides, ShapeVariant variant);
void SpawnChildShapes(GameData* GD, int parent);