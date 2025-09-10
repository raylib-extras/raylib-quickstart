#include "gs_draw.h"

#include <string.h>
#include <strings.h>

#include "camera_util.h"

void GsDrawCheckerboard(GameScene* GS) {
  const int check_size = 128;
  const fixed_t check_z = default_z;

  fixed_t check_center_x = (uint32_t)GS->player.x / fixed_new(check_size, 0) / 2 * fixed_new(check_size, 0) * 2;
  fixed_t check_center_y = (uint32_t)GS->player.y / fixed_new(check_size, 0) / 2 * fixed_new(check_size, 0) * 2;

  for (int yidx = -4; yidx <= 3; ++yidx) {
    for (int xidx = -3; xidx <= 2; ++xidx) {
      fixed_t check_x = check_center_x - (xidx * fixed_new(check_size, 0) * 2);
      fixed_t check_y = check_center_y - (yidx * fixed_new(check_size, 0));
      if (yidx % 2 == 0) {
        check_x -= fixed_new(check_size, 0);
      }

      int render_x, render_y;
      GetRenderCoords(GS, check_x, check_y, check_z, &render_x, &render_y);

      int render_check_size = GetRenderLength(GS, check_size, check_z);

      DrawRectangle(render_x, render_y, render_check_size, render_check_size, LIGHTGRAY);
    }
  }
}

void GsDrawProjs(GameScene* GS) {
  for (int p = 0; p < LENGTHOF(GS->projs); ++p) {
    if (!GS->projs[p].exists) {
      continue;
    }
    int rx, ry;
    GetRenderCoords(GS, GS->projs[p].x, GS->projs[p].y, default_z, &rx, &ry);
    Vector2 render_pos = {rx, ry};
    int render_size = GetRenderLength(GS, GS->projs[p].size, default_z);
    if (render_size < 2) {
      render_size = 2;
    }
    Color color = (GS->projs[p].is_homing && GS->projs[p].despawn_timer % 2 == 0 ? MAGENTA : GRAY);
    DrawPoly(render_pos, 4, render_size, GS->ticks + p * 15, color);
    DrawPolyLinesEx(render_pos, 4, render_size, GS->ticks + p * 15, 2.0f, BLACK);
  }
}

void GsDrawShapes(GameScene* GS) {
  for (int i = 0; i < LENGTHOF(GS->shapes); ++i) {
    if (!GS->shapes[i].exists) {
      continue;
    }
    int rx, ry;
    GetRenderCoords(GS, GS->shapes[i].x, GS->shapes[i].y, default_z, &rx, &ry);
    Vector2 render_pos = {rx, ry};
    // shape
    Color fg = (GS->shapes[i].ticks_since_damaged >= 8) ? GS->shapes[i].fg : WHITE;
    Color bg = (GS->shapes[i].ticks_since_damaged >= 8) ? GS->shapes[i].bg : PINK;
    int rotation = GS->ticks;
    if (GS->shapes[i].variant == SHAPE_VARIANT_FAST) {
      rotation = GS->ticks * 3;
    }
    if (GS->shapes[i].variant == SHAPE_VARIANT_HEALING) {
      rotation = -GS->ticks;
    }
    int render_size = GetRenderLength(GS, GS->shapes[i].size, default_z);
    if (render_size < 3) {
      render_size = 3;
    }
    DrawPoly(render_pos, GS->shapes[i].sides, render_size, rotation, fg);
    DrawPolyLinesEx(render_pos, GS->shapes[i].sides, render_size, rotation, 2.0f, bg);

    // healthbar
    if (GS->shapes[i].hp < GS->shapes[i].max_hp) {
      int bar_width = GetRenderLength(GS, GS->shapes[i].max_hp / 20, default_z);
      int filled_width = bar_width * GS->shapes[i].hp / GS->shapes[i].max_hp;
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
    //          render_pos.x + fixed_whole(fixed_cos(GS->shapes[i].move_angle) * GS->shapes[i].move_speed * target_fps / fixed_factor),
    //          render_pos.y + fixed_whole(fixed_sin(GS->shapes[i].move_angle) * GS->shapes[i].move_speed * target_fps / fixed_factor),
    //          RED);
    // DrawLine(render_pos.x, render_pos.y,
    //          render_pos.x + fixed_whole(fixed_cos(GS->shapes[i].kb_angle) * GS->shapes[i].kb_speed * target_fps / fixed_factor),
    //          render_pos.y + fixed_whole(fixed_sin(GS->shapes[i].kb_angle) * GS->shapes[i].kb_speed * target_fps / fixed_factor),
    //          BLUE);
  }
}

// hack
int GsXpForLevelUp(GameScene* GS);

void GsDrawPlayer(GameScene* GS) {
  int rx, ry;
  GetRenderCoords(GS, GS->player.x, GS->player.y, default_z, &rx, &ry);
  Vector2 render_pos = {rx, ry};
  int cannon_distance = GS->player.stats.size * 3 / 4 + GS->player.ticks_since_last_shot;
  clamp(&cannon_distance, GS->player.stats.size * 3 / 4, GS->player.stats.size * 5 / 4);
  GetRenderCoords(GS,
                  GS->player.x + fixed_cos(GS->player.angle) * cannon_distance,
                  GS->player.y + fixed_sin(GS->player.angle) * cannon_distance,
                  default_z,
                  &rx, &ry);
  Vector2 cannon_pos = {rx, ry};
  Color fg = (GS->player.ticks_since_damaged <= 30 && GS->player.ticks_since_damaged % 4 < 2) ? WHITE : GRAY;
  Color bg = (GS->player.ticks_since_damaged <= 30 && GS->player.ticks_since_damaged % 4 < 2) ? PINK : BLACK;

  DrawPoly(cannon_pos, 4, GetRenderLength(GS, GS->player.stats.size * 3 / 5, default_z), (32 + GS->player.angle) * 360 / angle_factor, fg);
  DrawPolyLinesEx(cannon_pos, 4, GetRenderLength(GS, GS->player.stats.size * 3 / 5, default_z), (32 + GS->player.angle) * 360 / angle_factor, 2.5f, bg);

  DrawPoly(render_pos, 20, GetRenderLength(GS, GS->player.stats.size, default_z), 0, fg);
  DrawPolyLinesEx(render_pos, 20, GetRenderLength(GS, GS->player.stats.size, default_z), 0, 2.0f, bg);

  // DrawLine(render_pos.x,
  //          render_pos.y,
  //          render_pos.x + fixed_whole(fixed_cos(GS->player.angle) * 16),
  //          render_pos.y + fixed_whole(fixed_sin(GS->player.angle) * 16),
  //          GREEN);

  // healthbar
  if (GS->player.hp < GS->player.stats.max_hp) {
    int bar_width = GetRenderLength(GS, GS->player.stats.max_hp / 20, default_z);
    int filled_width = bar_width * GS->player.hp / GS->player.stats.max_hp;
    Color color = GREEN;
    if (filled_width <= bar_width / 2) {
      color = YELLOW;
    }
    if (filled_width <= bar_width / 4) {
      color = RED;
    }
    DrawRectangle(render_pos.x - bar_width / 2 - 1, render_pos.y + GS->player.stats.size + 2 - 1, bar_width + 2, 5, BLACK);
    DrawRectangle(render_pos.x - bar_width / 2, render_pos.y + GS->player.stats.size + 2, filled_width, 3, color);
  }
}

void GsDrawPickups(GameScene* GS) {
  for (int p = 0; p < LENGTHOF(GS->pickups); ++p) {
    if (!GS->pickups[p].exists) {
      continue;
    }
    int rx, ry;
    GetRenderCoords(GS, GS->pickups[p].x, GS->pickups[p].y, default_z, &rx, &ry);
    Vector2 render_pos = {rx, ry};
    int render_size = GetRenderLength(GS, 12, default_z);
    if (render_size < 6) {
      render_size = 6;
    }

    DrawPoly(render_pos, 4, render_size, 0, WHITE);
    DrawPolyLinesEx(render_pos, 4, render_size, 0, 2.0f, SKYBLUE);
    DrawPrintf(render_pos.x - 3 * strlen(item_strs[GS->pickups[p].type]), render_pos.y, BLACK, "%s", item_strs[GS->pickups[p].type]);
    if (fixed_abs(GS->pickups[p].x - GS->player.x) > fixed_new(render_w / 2, 0) ||
        fixed_abs(GS->pickups[p].y - GS->player.y) > fixed_new(render_h / 2, 0)) {
      Vector2 marker_pos = {fixed_whole(GS->player.x - GS->camera.x) + render_w / 2,
                            fixed_whole(GS->player.y - GS->camera.y) + render_h / 2};
      marker_pos.x += fixed_whole(fixed_cos(GS->pickups[p].angle_to_player) * -150);
      marker_pos.y += fixed_whole(fixed_sin(GS->pickups[p].angle_to_player) * -110);
      DrawPrintf(marker_pos.x, marker_pos.y, BLACK, "!");
    }
  }
}

void GsDrawTextFx(GameScene* GS) {
  for (int t = 0; t < LENGTHOF(GS->text_fx); ++t) {
    if (!GS->text_fx[t].exists) {
      continue;
    }
    int rx, ry;
    GetRenderCoords(GS, GS->text_fx[t].x, GS->text_fx[t].y, default_z, &rx, &ry);
    Vector2 render_pos = {rx, ry};
    // DrawPrintf(render_pos.x - 3 * strlen(GS->text_fx[t].text), render_pos.y + 1, WHITE, "%s", GS->text_fx[t].text);
    DrawPrintf(render_pos.x - 3 * strlen(GS->text_fx[t].text), render_pos.y, BLACK, "%s", GS->text_fx[t].text);
  }
}

void GsDrawXpOrbs(GameScene* GS) {
  for (int o = 0; o < LENGTHOF(GS->xp_orbs); ++o) {
    if (!GS->xp_orbs[o].exists) {
      continue;
    }
    int rx, ry;
    GetRenderCoords(GS, GS->xp_orbs[o].x, GS->xp_orbs[o].y, default_z, &rx, &ry);
    Vector2 render_pos = {rx, ry};
    Color colors[] = {GREEN, LIME, GREEN, GREEN, LIME, GREEN};
    int size = GetRenderLength(GS, GS->xp_orbs[o].xp / 2, default_z);
    DrawCircleV(render_pos, ((size < 2) ? 2 : size), colors[GS->xp_orbs[o].age % LENGTHOF(colors)]);
  }
}

void GsDrawUi(GameScene* GS) {
  {
    int bar_width = render_w;
    int target_filled_width = bar_width * GS->player.xp / GsXpForLevelUp(GS);
    GS->draw_data.xp_bar_filled_width = fixed_lerp(GS->draw_data.xp_bar_filled_width, fixed_new(target_filled_width, 0), fixed_new(0, 32));
    Color color = MAGENTA;
    DrawRectangle(0, render_h - 4, bar_width, 4, BLACK);
    DrawRectangle(0, render_h - 4, fixed_whole(GS->draw_data.xp_bar_filled_width), 4, color);
    DrawPrintf(0, render_h - 4 - ft_height, BLACK, "Level %d, %d/%d XP", GS->player.level, GS->player.xp, GsXpForLevelUp(GS));
  }
}

void GsDrawPickItemOverlay(GameScene* GS) {
  const int ol_x = render_w / 8;
  const int ol_y = render_h / 8;
  const int ol_w = render_w * 3 / 4;
  const int ol_h = render_h * 3 / 4;
  const int lh = ft_height;  // line height
  DrawRectangle(ol_x, ol_y, ol_w, ol_h, BLACK);

  DrawPrintf(ol_x + lh, ol_y + lh, YELLOW, "You are now level %d!", GS->player.level);
  DrawPrintf(ol_x + lh, ol_y + lh + lh, GRAY, "Pick an upgrade!");

  for (int i = 0; i < GS->ol_pick_item.item_count; ++i) {
    int text_x = ol_x + lh;
    int text_y = ol_y + lh * (4 + i * 4);

    bool is_selected = (i == GS->ol_pick_item.selected_item_idx);
    const char* prefix = (is_selected ? "> " : " ");

    char buf[256] = "";
    strcpy(buf, prefix);
    strcat(buf, "+");
    strcat(buf, item_strs[GS->ol_pick_item.choices[i].item_a]);
    if (GS->ol_pick_item.choices[i].item_b != ITEM_INVALID) {
      strcat(buf, "\n");
      strcat(buf, prefix);
      strcat(buf, "+");
      strcat(buf, item_strs[GS->ol_pick_item.choices[i].item_b]);
    }
    if (GS->ol_pick_item.choices[i].removed_item != ITEM_INVALID) {
      strcat(buf, "\n");
      strcat(buf, prefix);
      strcat(buf, "-");
      strcat(buf, item_strs[GS->ol_pick_item.choices[i].removed_item]);
    }
    DrawPrintf(text_x, text_y, WHITE, "%s", buf);
  }

  // Draw stat changes
  {
    int text_x = render_w / 2;
    int text_y = ol_y + lh * 4;
    for (GsPlayerStatType s = 0; s < STAT_COUNT; ++s) {
      if (GS->player.stats.as_int[s] != GS->player.tmp_stats.as_int[s]) {
        bool is_upgrade = stat_lower_is_better[s] == (GS->player.tmp_stats.as_int[s] <= GS->player.stats.as_int[s]);
        DrawPrintf(text_x, text_y, (is_upgrade ? LIME : RED),
                   "%s:\n  %d -> %d",
                   stat_names[s],
                   GS->player.stats.as_int[s],
                   GS->player.tmp_stats.as_int[s]);
        text_y += lh * 3;
      }
    }
  }

  DrawPrintf(ol_x + lh, ol_y + ol_h - lh * 3, GRAY, "W/S to select, \nSPACE to confirm");
}

void GsDraw(GameScene* GS) {
  // Always draw the main screen.
  GsDrawCheckerboard(GS);
  GsDrawProjs(GS);
  GsDrawShapes(GS);
  GsDrawPlayer(GS);
  GsDrawPickups(GS);
  GsDrawXpOrbs(GS);
  GsDrawUi(GS);
  GsDrawTextFx(GS);

  switch (GS->curr_overlay) {
    case GS_OVERLAY_NONE: {
    } break;

    case GS_OVERLAY_PICK_ITEM: {
      GsDrawPickItemOverlay(GS);
    } break;

    default: {
      TraceLog(LOG_ERROR, "Bad overlay value %d", GS->curr_overlay);
    } break;
  }
}