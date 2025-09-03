/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit
https://creativecommons.org/publicdomain/zero/1.0/

*/
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "camera_util.h"
#include "draw_logic.h"
#include "fixed_math.h"
#include "gd.h"
#include "perf.h"
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
  PerfInit();

  // Utility function from resource_dir.h to find the resources folder and set
  // it as the current working directory so we can load from it
  SearchAndSetResourceDir("resources");

  GameData game_data;

  GameData* GD = &game_data;
  InitGameData(GD);

  GameData save_state = game_data;
  int game_speed = 1;

  bool show_stats = false;

  RenderTexture2D canvas = LoadRenderTexture(render_w, render_h);

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_F1)) {
      save_state = game_data;
    } else if (IsKeyPressed(KEY_F2)) {
      game_data = save_state;

    } else if (IsKeyPressed(KEY_F3)) {
      show_stats = !show_stats;

    } else if (IsKeyPressed(KEY_F4)) {
      window_scale = (window_scale % 4) + 1;
      window_w = render_w * window_scale;
      window_h = render_h * window_scale;
      SetWindowSize(window_w, window_h);

    } else if (IsKeyPressed(KEY_F5)) {
      game_speed /= 2;
    } else if (IsKeyPressed(KEY_F6)) {
      if (game_speed < (1 << 8)) {
        game_speed *= 2;
      }
      if (game_speed == 0) {
        game_speed = 1;
      }

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

    } else if (IsKeyPressed(KEY_MINUS) || IsKeyPressedRepeat(KEY_MINUS)) {
      GD->camera.zoom -= 8;
    } else if (IsKeyPressed(KEY_EQUAL) || IsKeyPressedRepeat(KEY_EQUAL)) {
      GD->camera.zoom += 8;
    }

    // ------[Game Logic]------
    for (int t = 0; t < game_speed; ++t) {
      ++GD->ticks;

      PERF_EXPR("UPDATE", (SpawnNewShapes(GD),
                           UpdateShapes(GD),

                           UpdatePlayer(GD),

                           SpawnNewProjs(GD),
                           UpdateProjs(GD),

                           UpdatePickups(GD),

                           UpdateTextFx(GD),
                           UpdateXpOrbs(GD)));

      // update camera
      GD->camera.x = GD->player.x - GetRenderLength(GD, render_w / 2, default_z);
      GD->camera.y = GD->player.y - GetRenderLength(GD, render_h / 2, default_z);

      fixed_t target_zoom = 256 * 256 / (GD->player.stats.view_distance * fixed_factor * 2 / render_h);
      fixed_nudge(&GD->camera.zoom, target_zoom, 1);
    }

    // ------[Drawing]------
    BeginTextureMode(canvas);
    ClearBackground(WHITE);

    PERF_EXPR("DRAW", (DrawCheckerboard(GD),
                       DrawProjs(GD),
                       DrawShapes(GD),
                       DrawPlayer(GD),
                       DrawPickups(GD),
                       DrawXpOrbs(GD),
                       DrawTextFx(GD)));

    for (int i = 0; i < ITEM_COUNT; ++i) {
      DrawPrintf(i * 12, 0, BLACK, "%d", GD->player.item_counts[i]);
    }
    DrawPrintf(0, 8, BLACK, "%d pickups spawned", GD->pickups_spawned);
    DrawPrintf(0, 16, BLACK, "Lvl %d - XP %d/%d", GD->player.level, GD->player.xp, XpForLevelUp(GD));
    if (show_stats) {
      int i = 5;
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
      PRINT_STAT(i++, view_distance);
      // DrawPrintf(0, 0, BLACK, "x: %d\ny: %d\nzoom: %d", GD->camera.x, GD->camera.y, GD->camera.zoom);

      const int entries = LENGTHOF(perf_entries[0].us_entries);
      for (int x = 0; x < 120; ++x) {
        int y1 = render_h;
        for (int f = 0; f < LENGTHOF(perf_entries) && perf_entries[f].exists; ++f) {
          int us = perf_entries[f].us_entries[x] / 16;
          Color color = (f % 2 == 0 ? GRAY : SKYBLUE);
          if (x != perf_entries[f].us_index) {
            DrawLine(x, y1, x, y1 - us, color);
          }
          y1 -= us;
        }
      }
    }

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
  PerfUninit();

  return 0;
}
