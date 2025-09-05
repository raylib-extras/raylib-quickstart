#include "gd_update.h"

#include "gd.h"
#include "gs_update.h"

void GdUpdate(GameData* GD) {
  GsUpdate(&GD->GS);
}
