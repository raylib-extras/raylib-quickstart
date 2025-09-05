#include "camera_util.h"

// Returns the screen length of a distance `length` at distance `z` with respect to the camera.
int GetRenderLength(GameScene* GS, int length, fixed_t z) {
  return (length * GS->camera.zoom / z);
}

void GetRenderCoords(GameScene* GS, fixed_t x, fixed_t y, fixed_t z, int* ox, int* oy) {
  // translation
  fixed_t tx = x - GS->camera.x;
  fixed_t ty = y - GS->camera.y;

  // zoom
  int itx = GetRenderLength(GS, tx, z) / fixed_factor;
  int ity = GetRenderLength(GS, ty, z) / fixed_factor;

  *ox = itx + render_w / 2;
  *oy = ity + render_h / 2;
}
