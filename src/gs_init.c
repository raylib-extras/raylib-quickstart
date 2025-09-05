#include "gs_init.h"

#include <raylib.h>

#include "fixed_math.h"
#include "gs.h"
#include "gs_update.h"

void GsInit(GameScene* GS) {
  GsUpdatePlayerStats(GS);
  GS->player.hp = GS->player.stats.max_hp;
  GS->camera.zoom = fixed_new(1, 0);
  GS->font = LoadFontEx("Kitchen Sink.ttf", 8, NULL, 0);
}
