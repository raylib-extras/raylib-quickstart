#include "gd_init.h"

#include "gd.h"
#include "gs_init.h"

void GdInit(GameData* GD) {
  *GD = (GameData){0};
  GsInit(&GD->GS);
}
