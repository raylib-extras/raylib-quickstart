/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit
https://creativecommons.org/publicdomain/zero/1.0/

*/
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "camera_util.h"
#include "gd.h"
#include "gd_draw.h"
#include "gd_update.h"
#include "math_util.h"
#include "perf.h"
#include "resource_dir.h"  // utility header for SearchAndSetResourceDir

int main() {
  SetTraceLogLevel(LOG_WARNING);
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
  InitWindow(window_w, window_h, "raygame");
  SetTargetFPS(target_fps);
  PerfInit();

  // Utility function from resource_dir.h to find the resources folder and set
  // it as the current working directory so we can load from it
  SearchAndSetResourceDir("resources");
  ft_font = LoadFontEx("Kitchen Sink.ttf", ft_height, NULL, 0);
  SetTextLineSpacing(1);
  GameData game_data;

  GameData* GD = &game_data;
  GdInit(GD);
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
      for (int p = 0; p < LENGTHOF(GD->GS.pickups); ++p) {
        if (GD->GS.pickups[p].exists) {
          GD->GS.pickups[p].type = (GD->GS.pickups[p].type - 1 + ITEM_COUNT) % ITEM_COUNT;
        }
      }
    } else if (IsKeyPressed(KEY_F10)) {
      for (int p = 0; p < LENGTHOF(GD->GS.pickups); ++p) {
        if (GD->GS.pickups[p].exists) {
          GD->GS.pickups[p].type = (GD->GS.pickups[p].type + 1) % ITEM_COUNT;
        }
      }

    } else if (IsKeyPressed(KEY_MINUS) || IsKeyPressedRepeat(KEY_MINUS)) {
      GD->GS.camera.zoom -= 8;
    } else if (IsKeyPressed(KEY_EQUAL) || IsKeyPressedRepeat(KEY_EQUAL)) {
      GD->GS.camera.zoom += 8;
    }

    // ------[Game Logic]------
    PerfStart("UPDATE");
    for (int t = 0; t < game_speed; ++t) {
      GdUpdate(GD);
    }
    PerfEnd("UPDATE");

    // ------[Drawing]------
    BeginTextureMode(canvas);
    ClearBackground(WHITE);

    PERF_EXPR("DRAW", GdDraw(GD));

    GameScene* GS = &GD->GS;
    for (int i = 0; i < ITEM_COUNT; ++i) {
      DrawPrintf(i * 12, 0, BLACK, "%d", GD->GS.player.item_counts[i]);
    }
    DrawPrintf(0, ft_height, BLACK, "%d pickups spawned", GD->GS.pickups_spawned);
    // DrawPrintf(0, 16, BLACK, "Lvl %d - XP %d/%d", GD->GS.player.level, GD->GS.player.xp, GsXpForLevelUp(GS));
    if (show_stats) {
      int l = 2;
      for (int i = 0; i < STAT_COUNT; ++i) {
        DrawPrintf(0, ft_height * l, BLACK, "%s: %d", stat_names[i], GS->player.stats.as_int[i]);
        ++l;
      }
      // DrawPrintf(0, 0, BLACK, "x: %d\ny: %d\nzoom: %d", GD->GS.camera.x, GD->GS.camera.y, GD->GS.camera.zoom);

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
