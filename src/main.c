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

#define FIELD(obj, field) ((obj).field)
#define PRINT_STAT(index, name)                                                                                                     \
  do {                                                                                                                              \
    if (FIELD(GD->player.stats, name) != FIELD(GD->player.prev_stats, name)) {                                                      \
      DrawPrintf(0, (index) * 8, VIOLET, "%s: %d -> %d", #name, FIELD(GD->player.prev_stats, name), FIELD(GD->player.stats, name)); \
    } else {                                                                                                                        \
      DrawPrintf(0, (index) * 8, BLACK, "%s: %d", #name, FIELD(GD->player.stats, name));                                            \
    }                                                                                                                               \
  } while (0);

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

  GameData save_state = game_data;
  int game_speed = 1;

  RenderTexture2D canvas = LoadRenderTexture(render_w, render_h);

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_F1)) {
      save_state = game_data;
    } else if (IsKeyPressed(KEY_F2)) {
      game_data = save_state;
    } else if (IsKeyPressed(KEY_F5)) {
      game_speed = 0;
    } else if (IsKeyPressed(KEY_F6)) {
      game_speed = 1;
    } else if (IsKeyPressed(KEY_F7)) {
      game_speed = 2;
    } else if (IsKeyPressed(KEY_F8)) {
      game_speed = 100;
    } else if (IsKeyPressed(KEY_F9)) {
      for (int p = 0; p < LENGTHOF(GD->pickups); ++p) {
        if (GD->pickups[p].exists) {
          GD->pickups[p].type = (GD->pickups[p].type - 1 + ITEM_COUNT) % ITEM_COUNT;
        }
      }
    } else if (IsKeyPressed(KEY_F10)) {
      for (int p = 0; p < LENGTHOF(GD->pickups); ++p) {
        if (GD->pickups[p].exists) {
          GD->pickups[p].type = (GD->pickups[p].type + 1) % ITEM_COUNT;
        }
      }
    }

    // ------[Game Logic]------
    for (int t = 0; t < game_speed; ++t) {
      ++GD->ticks;

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
    }

    // ------[Drawing]------
    BeginTextureMode(canvas);
    ClearBackground(WHITE);

    DrawCheckerboard(GD);
    DrawProjs(GD);
    DrawShapes(GD);
    DrawPlayer(GD);
    DrawPickups(GD);
    DrawTextFx(GD);

    int i = 0;
    PRINT_STAT(i++, damage);
    PRINT_STAT(i++, max_move_speed);
    PRINT_STAT(i++, reload_delay);
    PRINT_STAT(i++, shot_count);
    PRINT_STAT(i++, shot_kb);
    PRINT_STAT(i++, shot_pierce);
    PRINT_STAT(i++, shot_speed);
    PRINT_STAT(i++, shot_spread);
    PRINT_STAT(i++, sight_range);
    PRINT_STAT(i++, size);
    PRINT_STAT(i++, turn_speed);
    PRINT_STAT(i++, magnetism_dist);
    PRINT_STAT(i++, shot_homing_power);
    EndTextureMode();
    BeginDrawing();
    DrawTexturePro(canvas.texture, (Rectangle){0, 0, canvas.texture.width, -canvas.texture.height},
                   (Rectangle){0, 0, window_w, window_h}, (Vector2){0, 0}, 0.0f, WHITE);
    EndDrawing();
  }

  // cleanup
  // unload our texture so it can be cleaned up
  UnloadRenderTexture(canvas);

  // destroy the window and cleanup the OpenGL context
  CloseWindow();
  return 0;
}
