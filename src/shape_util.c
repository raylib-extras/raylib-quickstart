#include "shape_util.h"

int CompareShapes(const void* p, const void* q) {
  const GsShape* a = p;
  const GsShape* b = q;
  int exists_cmp = a->exists - b->exists;
  if (exists_cmp != 0) {
    return -exists_cmp;
  }
  return (a->y - b->y);
}

uint8_t next_free_id = 1;
int ClaimEmptyShapeSlot(GameScene* GS) {
  if (GS->shape_count >= LENGTHOF(GS->shapes)) {
    return -1;
  }
  for (int i = 0; i < LENGTHOF(GS->shapes); ++i) {
    if (!GS->shapes[i].exists) {
      ++GS->shape_count;
      GS->shapes[i] = (GsShape){0};
      GS->shapes[i].exists = true;
      GS->shapes[i].id = next_free_id;
      ++next_free_id;
      if (next_free_id == 0) {  // skip 0
        ++next_free_id;
      }
      return i;
    }
  }
  return -1;
}

int PickShapeSides(GameScene* GS) {
  int r = GetRandomValue(0, 99);
  if (GS->player.level < 10) {
    if (r < 85) {
      return 3;
    } else {
      return 4;
    }
  } else if (GS->player.level < 20) {
    if (r < 75) {
      return 3;
    } else if (r < 90) {
      return 4;
    } else {
      return 5;
    }
  } else if (GS->player.level < 40) {
    if (r < 75) {
      return 3;
    } else if (r < 90) {
      return 4;
    } else if (r < 98) {
      return 5;
    } else {
      return 6;
    }
  } else if (GS->player.level < 60) {
    if (r < 50) {
      return 3;
    } else if (r < 80) {
      return 4;
    } else if (r < 95) {
      return 5;
    } else {
      return 6;
    }
  } else {
    if (r < 50) {
      return 4;
    } else if (r < 75) {
      return 5;
    } else {
      return 6;
    };
  }
}

GsShapeVariant PickShapeVariant(GameScene* GS, int sides) {
  int r = GetRandomValue(0, 99);
  if (GS->player.level < 30) {
    return SHAPE_VARIANT_NONE;
  } else if (GS->player.level < 50) {
    if (r < 90) {
      return SHAPE_VARIANT_NONE;
    } else {
      return SHAPE_VARIANT_BIG;
    }
  } else if (GS->player.level < 70) {
    if (r < 90) {
      return SHAPE_VARIANT_NONE;
    } else if (r < 95) {
      return SHAPE_VARIANT_BIG;
    } else {
      return SHAPE_VARIANT_FAST;
    }
  } else {
    if (r < 85) {
      return SHAPE_VARIANT_NONE;
    } else if (r < 90) {
      return SHAPE_VARIANT_BIG;
    } else if (r < 95) {
      return SHAPE_VARIANT_FAST;
    } else {
      return SHAPE_VARIANT_HEALING;
    }
  }
}

void SetShapeStats(GsShape* shape, int sides, GsShapeVariant variant) {
  shape->sides = sides;
  shape->variant = variant;
  switch (sides) {
    case 3: {
      shape->max_hp = 200;
      shape->max_move_speed = fixed_new(64, 0) / target_fps;
      shape->regen = 0;
      shape->sides = 3;
      shape->size = 8;
      shape->fg = YELLOW;
      shape->bg = GOLD;
      shape->spawn_children_on_despawn = false;
      shape->xp = 4;
    } break;
    case 4: {
      shape->max_hp = 400;
      shape->max_move_speed = fixed_new(24, 0) / target_fps;
      shape->regen = 4;
      shape->sides = 4;
      shape->size = 12;
      shape->fg = PINK;
      shape->bg = RED;
      shape->spawn_children_on_despawn = true;
      shape->xp = 8;
    } break;
    case 5: {
      shape->max_hp = 1000;
      shape->max_move_speed = fixed_new(16, 0) / target_fps;
      shape->regen = 12;
      shape->sides = 5;
      shape->size = 24;
      shape->fg = PURPLE;
      shape->bg = VIOLET;
      shape->spawn_children_on_despawn = true;
      shape->xp = 16;
    } break;
    case 6: {
      shape->max_hp = 5000;
      shape->max_move_speed = fixed_new(16, 0) / target_fps;
      shape->regen = 50;
      shape->sides = 6;
      shape->size = 40;
      shape->fg = GREEN;
      shape->bg = LIME;
      shape->spawn_children_on_despawn = true;
      shape->xp = 32;
    } break;
    default: {
      TraceLog(LOG_ERROR, "Bad Shape side count of %d", sides);
    } break;
  }

  switch (variant) {
    case SHAPE_VARIANT_NONE: {
    } break;
    case SHAPE_VARIANT_BIG: {
      shape->fg = DARKGRAY;
      shape->bg = BLACK;
      shape->size *= 2;
      shape->max_hp *= 4;
      shape->regen /= 2;
      shape->max_move_speed;
      shape->xp *= 2;
    } break;
    case SHAPE_VARIANT_FAST: {
      shape->fg = ORANGE;
      shape->bg = BLACK;
      shape->max_hp *= 2;
      shape->regen = 0;
      shape->max_move_speed *= 3;
      shape->xp *= 2;
    } break;
    case SHAPE_VARIANT_HEALING: {
      shape->fg = RED;
      shape->bg = WHITE;
      shape->max_hp *= 2;
      shape->regen += 10;
      shape->xp *= 2;
    } break;
    default: {
      TraceLog(LOG_ERROR, "Bad Shape side count of %d", sides);
    } break;
  }

  shape->hp = shape->max_hp;
  shape->marked_for_despawn = false;
}

void SpawnChildShapes(GameScene* GS, int parent) {
  int parent_sides = GS->shapes[parent].sides;
  int child_count = parent_sides * (GS->shapes[parent].variant == SHAPE_VARIANT_BIG ? 2 : 1);
  angle_t ang = GetRandomValue(0, angle_factor - 1);
  for (int i = 0; i < child_count; ++i) {
    ang += (angle_factor / child_count);
    int s = ClaimEmptyShapeSlot(GS);
    if (s == -1) {
      return;
    }
    GS->shapes[s].x = GS->shapes[parent].x;
    GS->shapes[s].y = GS->shapes[parent].y;
    GS->shapes[s].kb_speed = fixed_new(75, 0) / target_fps;
    GS->shapes[s].kb_angle = ang;
    GS->shapes[s].i_frames = 30;
    GS->shapes[s].ticks_since_damaged = 0;
    SetShapeStats(&GS->shapes[s], parent_sides - 1, GS->shapes[parent].variant);
    GS->shapes[s].hp = GS->shapes[s].max_hp / 2;
  }
}
