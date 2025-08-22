#include "draw_logic.h"

#include <string.h>
#include <strings.h>

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
  for (int p = 0; p < LENGTHOF(GD->projs); ++p) {
    if (!GD->projs[p].exists) {
      continue;
    }
    Vector2 render_pos = {fixed_whole(GD->projs[p].x - GD->camera.x) + render_w / 2,
                          fixed_whole(GD->projs[p].y - GD->camera.y) + render_h / 2};
    Color color = (GD->projs[p].is_homing && GD->projs[p].despawn_timer % 2 == 0 ? MAGENTA : GRAY);
    DrawPoly(render_pos, 4, GD->projs[p].size, GD->ticks + p * 15, color);
    DrawPolyLinesEx(render_pos, 4, GD->projs[p].size, GD->ticks + p * 15, 2.0f, BLACK);
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
    int rotation = GD->ticks;
    if (GD->shapes[i].variant == SHAPE_VARIANT_FAST) {
      rotation = GD->ticks * 3;
    }
    if (GD->shapes[i].variant == SHAPE_VARIANT_HEALING) {
      rotation = -GD->ticks;
    }
    DrawPoly(render_pos, GD->shapes[i].sides, GD->shapes[i].size, rotation, fg);
    DrawPolyLinesEx(render_pos, GD->shapes[i].sides, GD->shapes[i].size, rotation, 2.0f, bg);

    // healthbar
    if (GD->shapes[i].hp < GD->shapes[i].max_hp) {
      int bar_width = GD->shapes[i].max_hp / 20;
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
    // DrawPrintf(render_pos.x, render_pos.y, BLACK, "[%d]", i);
    DrawLine(render_pos.x, render_pos.y,
             render_pos.x + fixed_whole(fixed_cos(GD->shapes[i].move_angle) * GD->shapes[i].move_speed * target_fps / fixed_factor),
             render_pos.y + fixed_whole(fixed_sin(GD->shapes[i].move_angle) * GD->shapes[i].move_speed * target_fps / fixed_factor),
             RED);
    DrawLine(render_pos.x, render_pos.y,
             render_pos.x + fixed_whole(fixed_cos(GD->shapes[i].kb_angle) * GD->shapes[i].kb_speed * target_fps / fixed_factor),
             render_pos.y + fixed_whole(fixed_sin(GD->shapes[i].kb_angle) * GD->shapes[i].kb_speed * target_fps / fixed_factor),
             BLUE);
  }
}

void DrawPlayer(GameData* GD) {
  Vector2 render_pos = {fixed_whole(GD->player.x - GD->camera.x) + render_w / 2,
                        fixed_whole(GD->player.y - GD->camera.y) + render_h / 2};
  DrawPoly(render_pos, 20, GD->player.stats.size, 0, GRAY);
  DrawPolyLinesEx(render_pos, 20, GD->player.stats.size, 0, 2.0f, BLACK);
  DrawLine(render_pos.x,
           render_pos.y,
           render_pos.x + fixed_whole(fixed_cos(GD->player.angle) * 16),
           render_pos.y + fixed_whole(fixed_sin(GD->player.angle) * 16),
           GREEN);
}

void DrawPickups(GameData* GD) {
  for (int p = 0; p < LENGTHOF(GD->pickups); ++p) {
    if (!GD->pickups[p].exists) {
      continue;
    }
    Vector2 render_pos = {fixed_whole(GD->pickups[p].x - GD->camera.x) + render_w / 2,
                          fixed_whole(GD->pickups[p].y - GD->camera.y) + render_h / 2};
    DrawPoly(render_pos, 4, 12, 0, WHITE);
    DrawPolyLinesEx(render_pos, 4, 12, 0, 2.0f, SKYBLUE);
    DrawPrintf(render_pos.x - 3 * strlen(item_strs[GD->pickups[p].type]), render_pos.y, BLACK, "%s", item_strs[GD->pickups[p].type]);
    if (fixed_abs(GD->pickups[p].x - GD->player.x) > fixed_new(render_w / 2, 0) ||
        fixed_abs(GD->pickups[p].y - GD->player.y) > fixed_new(render_h / 2, 0)) {
      Vector2 marker_pos = {fixed_whole(GD->player.x - GD->camera.x) + render_w / 2,
                            fixed_whole(GD->player.y - GD->camera.y) + render_h / 2};
      marker_pos.x += fixed_whole(fixed_cos(GD->pickups[p].angle_to_player) * -150);
      marker_pos.y += fixed_whole(fixed_sin(GD->pickups[p].angle_to_player) * -110);
      DrawPrintf(marker_pos.x, marker_pos.y, BLACK, "!");
    }
  }
}

void DrawTextFx(GameData* GD) {
  for (int t = 0; t < LENGTHOF(GD->text_fx); ++t) {
    if (!GD->text_fx[t].exists) {
      continue;
    }
    Vector2 render_pos = {fixed_whole(GD->text_fx[t].x - GD->camera.x) + render_w / 2,
                          fixed_whole(GD->text_fx[t].y - GD->camera.y) + render_h / 2};
    // DrawPrintf(render_pos.x - 3 * strlen(GD->text_fx[t].text), render_pos.y + 1, WHITE, "%s", GD->text_fx[t].text);
    DrawPrintf(render_pos.x - 3 * strlen(GD->text_fx[t].text), render_pos.y, BLACK, "%s", GD->text_fx[t].text);
  }
}
