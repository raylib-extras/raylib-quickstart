/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit
https://creativecommons.org/publicdomain/zero/1.0/

*/
#include <stdint.h>
#include <stdio.h>

#include "draw_logic.h"
#include "fixed_math.h"
#include "gd.h"
#include "raylib.h"
#include "resource_dir.h"  // utility header for SearchAndSetResourceDir
#include "update_logic.h"

int main() {
  SetTraceLogLevel(LOG_WARNING);
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
  InitWindow(window_w, window_h, "raygame");
  SetTargetFPS(target_fps);

  // Utility function from resource_dir.h to find the resources folder and set
  // it as the current working directory so we can load from it
  SearchAndSetResourceDir("resources");

  GameData game_data = {};
  GameData* GD = &game_data;
  InitGameData(GD);

  RenderTexture2D canvas = LoadRenderTexture(render_w, render_h);

  while (!WindowShouldClose()) {
    // ------[Game Logic]------
    SpawnNewShapes(GD);
    UpdateShapes(GD);

    UpdatePlayer(GD);

    SpawnNewProjs(GD);
    UpdateProjs(GD);

    UpdatePickups(GD);

    UpdateTextFx(GD);

    // update camera
    GD->camera.x = GD->player.x - render_w / 2;
    GD->camera.y = GD->player.y - render_h / 2;

    // ------[Drawing]------
    BeginTextureMode(canvas);
    ClearBackground(WHITE);

    DrawCheckerboard(GD);
    DrawProjs(GD);
    DrawShapes(GD);
    DrawPlayer(GD);
    DrawPickups(GD);
    DrawTextFx(GD);

    DrawPrintf(0, 0, BLACK, "%d FPS\nSpeed : %d\nSight : %d\nTurn  : %d\nDelay : %d (%d)\nDamage: %d\nSpread: %d\nShotSp: %d\nKB    : %d\nDPS   : %d\nPierce: %d",
               GetFPS(),
               GD->player.max_move_speed,
               GD->player.sight_range,
               GD->player.turn_speed,
               GD->player.reload_delay, fixed_factor * fixed_factor / target_fps / GD->player.reload_delay,
               GD->player.damage,
               GD->player.shot_spread,
               GD->player.shot_speed,
               GD->player.shot_kb,
               GD->player.dps,
               GD->player.shot_pierce);
    EndTextureMode();
    BeginDrawing();
    DrawTexturePro(canvas.texture, (Rectangle){0, 0, canvas.texture.width, -canvas.texture.height},
                   (Rectangle){0, 0, window_w, window_h}, (Vector2){0, 0}, 0.0f, WHITE);
    EndDrawing();

    ++GD->ticks;
  }

  // cleanup
  // unload our texture so it can be cleaned up
  UnloadRenderTexture(canvas);

  // destroy the window and cleanup the OpenGL context
  CloseWindow();
  return 0;
}
