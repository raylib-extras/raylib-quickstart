/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit
https://creativecommons.org/publicdomain/zero/1.0/

*/
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "fixed_math.h"
#include "gd.h"
#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"  // utility header for SearchAndSetResourceDir

int main() {
  SetTraceLogLevel(LOG_WARNING);
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
  InitWindow(window_w, window_h, "raygame");
  SetTargetFPS(60);

  // Utility function from resource_dir.h to find the resources folder and set
  // it as the current working directory so we can load from it
  SearchAndSetResourceDir("resources");

  GameData GD = {
      .player = {
          .size = 15,
          .sight_range = render_h / 2,
          .turn_speed = 2}};
  RenderTexture2D canvas = LoadRenderTexture(render_w, render_h);

  while (!WindowShouldClose()) {
    //
    // ------[Game Logic]------
    //

    // spawn new shapes
    if (GD.ticks % 4 == 0) {
      for (int i = 0; i < LENGTHOF(GD.shapes); ++i) {
        if (GD.shapes[i].exists) {
          continue;
        }
        GD.shapes[i].exists = true;
        GD.shapes[i].x = GD.player.x + fixed_new(GetRandomValue(-render_w, render_w), 0);
        GD.shapes[i].y = GD.player.y + fixed_new(GetRandomValue(-render_h, render_h), 0);
        GD.shapes[i].move_speed = fixed_new(GetRandomValue(1, 8), 0) / 60;
        GD.shapes[i].angle = GetRandomValue(0, angle_factor - 1);
        GD.shapes[i].sides = GetRandomValue(3, 6);
        GD.shapes[i].despawn_timer = GetRandomValue(1200, 1800);
        printf("Spawned shape %d\n", i);
        break;
      }
    }

    // shape logic
    for (int i = 0; i < LENGTHOF(GD.shapes); ++i) {
      if (!GD.shapes[i].exists) {
        continue;
      }
      // despawn timer
      --GD.shapes[i].despawn_timer;
      if (GD.shapes[i].despawn_timer <= 0) {
        printf("Despawned shape %d\n", i);
        GD.shapes[i].exists = false;
        continue;
      }
      // movement
      GD.shapes[i].x += fixed_cos(GD.shapes[i].angle) * GD.shapes[i].move_speed / fixed_factor;
      GD.shapes[i].y += fixed_sin(GD.shapes[i].angle) * GD.shapes[i].move_speed / fixed_factor;
      // sqdist
      int dx = abs(fixed_whole(GD.shapes[i].x) - fixed_whole(GD.player.x));
      int dy = abs(fixed_whole(GD.shapes[i].y) - fixed_whole(GD.player.y));
      GD.shapes[i].sqdist_to_player = (dx * dx) + (dy * dy);
      // absorption
      if (GD.shapes[i].sqdist_to_player < ((GD.player.size - 4) * (GD.player.size - 4))) {
        GD.player.size += 1;
        GD.shapes[i].exists = false;
      } else if (GD.shapes[i].sqdist_to_player < ((GD.player.size + 8) * (GD.player.size + 8))) {
        angle_t target_angle = angle_from_line(GD.shapes[i].x, GD.shapes[i].y, GD.player.x, GD.player.y);
        GD.shapes[i].angle = target_angle;
      }
    }

    // player shrinking
    if (GD.ticks % 120 == 0 && GD.player.size > 10) {
      --GD.player.size;
    }

    // player movement
    GD.player.move_speed = fixed_new(20, 0) / GD.player.size;
    if (IsKeyDown(KEY_A)) GD.player.x -= GD.player.move_speed;
    if (IsKeyDown(KEY_D)) GD.player.x += GD.player.move_speed;
    if (IsKeyDown(KEY_S)) GD.player.y += GD.player.move_speed;
    if (IsKeyDown(KEY_W)) GD.player.y -= GD.player.move_speed;

    // update player facing
    {
      int closest_sqdist = INT32_MAX;
      int closest_shape_idx = -1;
      for (int i = 0; i < LENGTHOF(GD.shapes); ++i) {
        if (GD.shapes[i].exists) {
          if (GD.shapes[i].sqdist_to_player < (GD.player.sight_range * GD.player.sight_range) &&
              GD.shapes[i].sqdist_to_player < closest_sqdist) {
            closest_shape_idx = i;
            closest_sqdist = GD.shapes[i].sqdist_to_player;
          }
        }
      }
      if (closest_shape_idx != -1) {
        angle_t target_angle = angle_from_line(GD.player.x, GD.player.y, GD.shapes[closest_shape_idx].x, GD.shapes[closest_shape_idx].y);
        int diff = angle_diff(GD.player.angle, target_angle);
        if (diff > GD.player.turn_speed)
          GD.player.angle += GD.player.turn_speed;
        else if (diff < -GD.player.turn_speed)
          GD.player.angle -= GD.player.turn_speed;
        else
          GD.player.angle += diff;
      }
    }

    // spawn new projs
    if (GD.ticks % 15 == 0) {
      for (int i = 0; i < LENGTHOF(GD.projs); ++i) {
        if (GD.projs[i].exists) {
          continue;
        }
        GD.projs[i].exists = true;
        GD.projs[i].x = GD.player.x;
        GD.projs[i].y = GD.player.y;
        GD.projs[i].move_speed = fixed_new(120, 0) / 60;
        GD.projs[i].angle = GD.player.angle;
        GD.projs[i].despawn_timer = 120;
        printf("Spawned proj %d\n", i);
        break;
      }
    }

    for (int i = 0; i < LENGTHOF(GD.projs); ++i) {
      if (!GD.projs[i].exists) {
        continue;
      }
      // despawn timer
      --GD.projs[i].despawn_timer;
      if (GD.projs[i].despawn_timer <= 0) {
        printf("Despawned proj %d\n", i);
        GD.projs[i].exists = false;
        continue;
      }
      // movement
      GD.projs[i].x += fixed_cos(GD.projs[i].angle) * GD.projs[i].move_speed / fixed_factor;
      GD.projs[i].y += fixed_sin(GD.projs[i].angle) * GD.projs[i].move_speed / fixed_factor;
    }

    // update camera
    GD.camera.x = GD.player.x - render_w / 2;
    GD.camera.y = GD.player.y - render_h / 2;
    printf("camera %d %d\n", GD.camera.x / fixed_factor, GD.camera.y / fixed_factor);

    //
    // ------[Drawing]------
    //
    BeginTextureMode(canvas);

    // Draw grid
    ClearBackground(WHITE);
    const int grid_size = 256;
    for (int y = -grid_size; y < window_h; ++y) {
      for (int x = -grid_size; x < window_w; ++x) {
        if (((x + fixed_whole(GD.camera.x)) & (grid_size - 1)) == 0 &&
            ((y + fixed_whole(GD.camera.y)) & (grid_size - 1)) == (grid_size >> 1)) {
          DrawRectangle(x, y, (grid_size >> 1), (grid_size >> 1), LIGHTGRAY);
        }
        if (((x + fixed_whole(GD.camera.x)) & (grid_size - 1)) == (grid_size >> 1) &&
            ((y + fixed_whole(GD.camera.y)) & (grid_size - 1)) == 0) {
          DrawRectangle(x, y, (grid_size >> 1), (grid_size >> 1), LIGHTGRAY);
        }
      }
    }

    // draw dotted red ring
    // for (int ang = 0; ang < angle_factor; ++ang) {
    //   DrawPixel(-fixed_whole(GD.camera.x) + fixed_cos(ang) / 2, -fixed_whole(GD.camera.y) + fixed_sin(ang) / 2, RED);
    // }

    // draw projs
    for (int i = 0; i < LENGTHOF(GD.projs); ++i) {
      if (!GD.projs[i].exists) {
        continue;
      }
      Vector2 render_pos = {fixed_whole(GD.projs[i].x - GD.camera.x) + render_w / 2,
                            fixed_whole(GD.projs[i].y - GD.camera.y) + render_h / 2};
      DrawPoly(render_pos, 4, 8, GD.ticks, GRAY);
      DrawPolyLinesEx(render_pos, 4, 8, GD.ticks, 2.0f, BLACK);
    }

    // draw shapes
    for (int i = 0; i < LENGTHOF(GD.shapes); ++i) {
      if (GD.shapes[i].exists && (GD.shapes[i].despawn_timer > 120 || GD.shapes[i].despawn_timer % 16 < 8)) {
        Vector2 render_pos = {fixed_whole(GD.shapes[i].x - GD.camera.x) + render_w / 2,
                              fixed_whole(GD.shapes[i].y - GD.camera.y) + render_h / 2};
        DrawPoly(render_pos, GD.shapes[i].sides, 8, GD.ticks, YELLOW);
        DrawPolyLinesEx(render_pos, GD.shapes[i].sides, 8, GD.ticks, 2.0f, GOLD);

        DrawPrintf(render_pos.x, render_pos.y, 10, BLACK, "[%d]", i);
        DrawLine(render_pos.x,
                 render_pos.y,
                 render_pos.x + fixed_whole(fixed_cos(GD.shapes[i].angle) * 16),
                 render_pos.y + fixed_whole(fixed_sin(GD.shapes[i].angle) * 16),
                 RED);
      }
    }

    // draw player
    {
      Vector2 render_pos = {fixed_whole(GD.player.x - GD.camera.x) + render_w / 2,
                            fixed_whole(GD.player.y - GD.camera.y) + render_h / 2};
      DrawPoly(render_pos, 20, GD.player.size, 0, GRAY);
      DrawPolyLinesEx(render_pos, 20, GD.player.size, 0, 2.0f, BLACK);
      DrawLine(render_pos.x,
               render_pos.y,
               render_pos.x + fixed_whole(fixed_cos(GD.player.angle) * 16),
               render_pos.y + fixed_whole(fixed_sin(GD.player.angle) * 16),
               GREEN);
    }
    DrawFPS(0, 0);

    EndTextureMode();
    BeginDrawing();
    DrawTexturePro(canvas.texture, (Rectangle){0, 0, canvas.texture.width, -canvas.texture.height},
                   (Rectangle){0, 0, window_w, window_h}, (Vector2){0, 0}, 0.0f, WHITE);
    EndDrawing();

    ++GD.ticks;
  }

  // cleanup
  // unload our texture so it can be cleaned up
  UnloadRenderTexture(canvas);

  // destroy the window and cleanup the OpenGL context
  CloseWindow();
  return 0;
}
