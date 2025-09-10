#pragma once

#include "gd.h"

int CompareShapes(const void* p, const void* q);
int ClaimEmptyShapeSlot(GameScene* GS);
int PickShapeSides(GameScene* GS);
GsShapeVariant PickShapeVariant(GameScene* GS, int sides);
void SetShapeStats(GameScene* GS, int s);
void SpawnChildShapes(GameScene* GS, int parent);