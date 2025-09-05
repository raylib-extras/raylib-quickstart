#pragma once

#include <raylib.h>

#include "fixed_math.h"
#include "gd.h"

void SpawnNewShapes(GameScene* GS);
void UpdateShapes(GameScene* GS);

int XpForLevelUp(GameScene* GS);

void UpdatePlayer(GameScene* GS);

void SpawnNewProjs(GameScene* GS);
void UpdateProjs(GameScene* GS);

void UpdatePickups(GameScene* GS);

void UpdateTextFx(GameScene* GS);
void UpdateXpOrbs(GameScene* GS);

void UpdateGsCamera(GameScene* GS);

void InitGameData(GameData* GD);
void UpdateGd(GameData* GD);
