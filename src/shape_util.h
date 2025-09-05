#pragma once

#include "gd.h"

int CompareShapes(const void* p, const void* q);
int ClaimEmptyShapeSlot(GameScene* GS);
int PickShapeSides(GameScene* GS);
GsShapeVariant PickShapeVariant(GameScene* GS, int sides);
void SetShapeStats(GsShape* shape, int sides, GsShapeVariant variant);
void SpawnChildShapes(GameScene* GS, int parent);