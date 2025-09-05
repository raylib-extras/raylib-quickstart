#pragma once

#include "gd.h"

void SpawnNewShapes(GameScene* GS);
int CompareShapes(const void* p, const void* q);
int ClaimEmptyShapeSlot(GameScene* GS);
int PickShapeSides(GameScene* GS);
ShapeVariant PickShapeVariant(GameScene* GS, int sides);
void SetShapeStats(Shape* shape, int sides, ShapeVariant variant);
void SpawnChildShapes(GameScene* GS, int parent);