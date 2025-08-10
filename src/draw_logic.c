#include "draw_logic.h"

void DrawCheckerboard(GameData* GD) {
  const int grid_size = 256;
  for (int y = -grid_size; y < window_h; ++y) {
    for (int x = -grid_size; x < window_w; ++x) {
      if (((x + fixed_whole(GD->camera.x)) & (grid_size - 1)) == 0 &&
          ((y + fixed_whole(GD->camera.y)) & (grid_size - 1)) == (grid_size >> 1)) {
        DrawRectangle(x, y, (grid_size >> 1), (grid_size >> 1), LIGHTGRAY);
      }
      if (((x + fixed_whole(GD->camera.x)) & (grid_size - 1)) == (grid_size >> 1) &&
          ((y + fixed_whole(GD->camera.y)) & (grid_size - 1)) == 0) {
        DrawRectangle(x, y, (grid_size >> 1), (grid_size >> 1), LIGHTGRAY);
      }
    }
  }
}

void DrawProjs(GameData* GD) {
  for (int i = 0; i < LENGTHOF(GD->projs); ++i) {
    if (!GD->projs[i].exists) {
      continue;
    }
    Vector2 render_pos = {fixed_whole(GD->projs[i].x - GD->camera.x) + render_w / 2,
                          fixed_whole(GD->projs[i].y - GD->camera.y) + render_h / 2};
    DrawPoly(render_pos, 4, 8, GD->ticks + i * 15, GRAY);
    DrawPolyLinesEx(render_pos, 4, 8, GD->ticks + i * 15, 2.0f, BLACK);
  }
}

void DrawShapes(GameData* GD) {
  for (int i = 0; i < LENGTHOF(GD->shapes); ++i) {
    if (!GD->shapes[i].exists) {
      continue;
    }
    Vector2 render_pos = {fixed_whole(GD->shapes[i].x - GD->camera.x) + render_w / 2,
                          fixed_whole(GD->shapes[i].y - GD->camera.y) + render_h / 2};
    // shape
    Color fg = (GD->shapes[i].ticks_since_damaged >= 4) ? GD->shapes[i].fg : WHITE;
    Color bg = (GD->shapes[i].ticks_since_damaged >= 4) ? GD->shapes[i].bg : PINK;
    DrawPoly(render_pos, GD->shapes[i].sides, GD->shapes[i].size, GD->ticks, fg);
    DrawPolyLinesEx(render_pos, GD->shapes[i].sides, GD->shapes[i].size, GD->ticks, 2.0f, bg);

    // healthbar
    if (GD->shapes[i].hp < GD->shapes[i].max_hp) {
      int bar_width = GD->shapes[i].max_hp / 2;
      int filled_width = bar_width * GD->shapes[i].hp / GD->shapes[i].max_hp;
      Color color = GREEN;
      if (filled_width <= bar_width / 2) {
        color = YELLOW;
      }
      if (filled_width <= bar_width / 4) {
        color = RED;
      }
      DrawRectangle(render_pos.x - bar_width / 2, render_pos.y + GD->shapes[i].size + 2, bar_width, 3, BLACK);
      DrawRectangle(render_pos.x - bar_width / 2, render_pos.y + GD->shapes[i].size + 2, filled_width, 3, color);
    }

    // debug info
    DrawPrintf(render_pos.x, render_pos.y, BLACK, "[%d]", i);
    DrawLine(render_pos.x,
             render_pos.y,
             render_pos.x + fixed_whole(fixed_cos(GD->shapes[i].angle) * 16),
             render_pos.y + fixed_whole(fixed_sin(GD->shapes[i].angle) * 16),
             RED);
  }
}

void DrawPlayer(GameData* GD) {
  Vector2 render_pos = {fixed_whole(GD->player.x - GD->camera.x) + render_w / 2,
                        fixed_whole(GD->player.y - GD->camera.y) + render_h / 2};
  DrawPoly(render_pos, 20, GD->player.size, 0, GRAY);
  DrawPolyLinesEx(render_pos, 20, GD->player.size, 0, 2.0f, BLACK);
  DrawLine(render_pos.x,
           render_pos.y,
           render_pos.x + fixed_whole(fixed_cos(GD->player.angle) * 16),
           render_pos.y + fixed_whole(fixed_sin(GD->player.angle) * 16),
           GREEN);
}