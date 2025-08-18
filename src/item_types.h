#pragma once

typedef enum ItemType {
  ITEM_SPEED_UP,
  ITEM_FIRE_RATE_UP,
  ITEM_TURN_SPEED_UP,
  ITEM_DAMAGE_UP,
  ITEM_ACCURACY_UP,
  ITEM_SHOT_SPEED_UP,
  ITEM_SHOT_COUNT_UP,
  ITEM_SHOT_KB_UP,
  ITEM_PIERCE_UP,
  ITEM_COUNT
} ItemType;

extern const char* const item_strs[];