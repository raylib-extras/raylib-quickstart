#include "draw_logic.h"

#include <string.h>
#include <strings.h>

#include "camera_transform.h"

void DrawCheckerboard(GameData* GD) {
  const int check_size = 128;
  const fixed_t check_z = default_z;

  fixed_t check_center_x = (uint32_t)GD->player.x / fixed_new(check_size, 0) / 2 * fixed_new(check_size, 0) * 2;
  fixed_t check_center_y = (uint32_t)GD->player.y / fixed_new(check_size, 0) / 2 * fixed_new(check_size, 0) * 2;

  for (int yidx = -4; yidx <= 3; ++yidx) {
    for (int xidx = -3; xidx <= 2; ++xidx) {
      fixed_t check_x = check_center_x - (xidx * fixed_new(check_size, 0) * 2);
      fixed_t check_y = check_center_y - (yidx * fixed_new(check_size, 0));
      if (yidx % 2 == 0) {
        check_x -= fixed_new(check_size, 0);
      }

      int render_x, render_y;
      GetRenderCoords(GD, check_x, check_y, check_z, &render_x, &render_y);

      int render_check_size = GetRenderLength(GD, check_size, check_z);

      DrawRectangle(render_x, render_y, render_check_size, render_check_size, LIGHTGRAY);
    }
  }
}

void DrawProjs(GameData* GD) {
  for (int p = 0; p < LENGTHOF(GD->projs); ++p) {
    if (!GD->projs[p].exists) {
      continue;
    }
    int rx, ry;
    GetRenderCoords(GD, GD->projs[p].x, GD->projs[p].y, default_z, &rx, &ry);
    Vector2 render_pos = {rx, ry};
    int render_size = GetRenderLength(GD, GD->projs[p].size, default_z);
    if (render_size < 2) {
      render_size = 2;
    }
    Color color = (GD->projs[p].is_homing && GD->projs[p].despawn_timer % 2 == 0 ? MAGENTA : GRAY);
    DrawPoly(render_pos, 4, render_size, GD->ticks + p * 15, color);
    DrawPolyLinesEx(render_pos, 4, render_size, GD->ticks + p * 15, 2.0f, BLACK);
  }
}

void DrawShapes(GameData* GD) {
  for (int i = 0; i < LENGTHOF(GD->shapes); ++i) {
    if (!GD->shapes[i].exists) {
      continue;
    }
    int rx, ry;
    GetRenderCoords(GD, GD->shapes[i].x, GD->shapes[i].y, default_z, &rx, &ry);
    Vector2 render_pos = {rx, ry};
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
    int render_size = GetRenderLength(GD, GD->shapes[i].size, default_z);
    if (render_size < 3) {
      render_size = 3;
    }
    DrawPoly(render_pos, GD->shapes[i].sides, render_size, rotation, fg);
    DrawPolyLinesEx(render_pos, GD->shapes[i].sides, render_size, rotation, 2.0f, bg);

    // healthbar
    if (GD->shapes[i].hp < GD->shapes[i].max_hp) {
      int bar_width = GetRenderLength(GD, GD->shapes[i].max_hp / 20, default_z);
      int filled_width = bar_width * GD->shapes[i].hp / GD->shapes[i].max_hp;
      Color color = GREEN;
      if (filled_width <= bar_width / 2) {
        color = YELLOW;
      }
      if (filled_width <= bar_width / 4) {
        color = RED;
      }
      DrawRectangle(render_pos.x - bar_width / 2, render_pos.y + render_size + 2, bar_width, 3, BLACK);
      DrawRectangle(render_pos.x - bar_width / 2, render_pos.y + render_size + 2, filled_width, 3, color);
    }

    // debug info
    // DrawPrintf(render_pos.x, render_pos.y, BLACK, "[%d]", i);
    // DrawLine(render_pos.x, render_pos.y,
    //          render_pos.x + fixed_whole(fixed_cos(GD->shapes[i].move_angle) * GD->shapes[i].move_speed * target_fps / fixed_factor),
    //          render_pos.y + fixed_whole(fixed_sin(GD->shapes[i].move_angle) * GD->shapes[i].move_speed * target_fps / fixed_factor),
    //          RED);
    // DrawLine(render_pos.x, render_pos.y,
    //          render_pos.x + fixed_whole(fixed_cos(GD->shapes[i].kb_angle) * GD->shapes[i].kb_speed * target_fps / fixed_factor),
    //          render_pos.y + fixed_whole(fixed_sin(GD->shapes[i].kb_angle) * GD->shapes[i].kb_speed * target_fps / fixed_factor),
    //          BLUE);
  }
}

void DrawPlayer(GameData* GD) {
  int rx, ry;
  GetRenderCoords(GD, GD->player.x, GD->player.y, default_z, &rx, &ry);
  Vector2 render_pos = {rx, ry};
  DrawPoly(render_pos, 20, GetRenderLength(GD, GD->player.stats.size, default_z), 0, GRAY);
  DrawPolyLinesEx(render_pos, 20, GetRenderLength(GD, GD->player.stats.size, default_z), 0, 2.0f, BLACK);
  // DrawLine(render_pos.x,
  //          render_pos.y,
  //          render_pos.x + fixed_whole(fixed_cos(GD->player.angle) * 16),
  //          render_pos.y + fixed_whole(fixed_sin(GD->player.angle) * 16),
  //          GREEN);
}

void DrawPickups(GameData* GD) {
  for (int p = 0; p < LENGTHOF(GD->pickups); ++p) {
    if (!GD->pickups[p].exists) {
      continue;
    }
    int rx, ry;
    GetRenderCoords(GD, GD->pickups[p].x, GD->pickups[p].y, default_z, &rx, &ry);
    Vector2 render_pos = {rx, ry};
    int render_size = GetRenderLength(GD, 12, default_z);
    if (render_size < 6) {
      render_size = 6;
    }

    DrawPoly(render_pos, 4, render_size, 0, WHITE);
    DrawPolyLinesEx(render_pos, 4, render_size, 0, 2.0f, SKYBLUE);
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
    int rx, ry;
    GetRenderCoords(GD, GD->text_fx[t].x, GD->text_fx[t].y, default_z, &rx, &ry);
    Vector2 render_pos = {rx, ry};
    // DrawPrintf(render_pos.x - 3 * strlen(GD->text_fx[t].text), render_pos.y + 1, WHITE, "%s", GD->text_fx[t].text);
    DrawPrintf(render_pos.x - 3 * strlen(GD->text_fx[t].text), render_pos.y, BLACK, "%s", GD->text_fx[t].text);
  }
}
