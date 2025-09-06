#include "gd_update.h"

#include "gd.h"
#include "gs_update.h"

void GdInit(GameData* GD) {
  *GD = (GameData){0};
  GsInit(&GD->GS);
}

void GdUpdate(GameData* GD) {
  GsUpdate(&GD->GS);
}
