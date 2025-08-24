#include "camera_util.h"

// Returns the screen length of a distance `length` at distance `z` with respect to the camera.
int GetRenderLength(GameData* GD, int length, fixed_t z) {
  return (length * GD->camera.zoom / z);
}

void GetRenderCoords(GameData* GD, fixed_t x, fixed_t y, fixed_t z, int* ox, int* oy) {
  // translation
  fixed_t tx = x - GD->camera.x;
  fixed_t ty = y - GD->camera.y;

  // zoom
  int itx = GetRenderLength(GD, tx, z) / fixed_factor;
  int ity = GetRenderLength(GD, ty, z) / fixed_factor;

  *ox = itx + render_w / 2;
  *oy = ity + render_h / 2;
}
