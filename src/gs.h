#pragma once

#include <raylib.h>

#include "fixed_math.h"
#include "item_types.h"

typedef struct GsPlayerStats {
  int max_hp;
  int regen_delay;
  int damage;
  fixed_t max_move_speed;
  fixed_t reload_delay;
  fixed_t shot_count;
  fixed_t shot_kb;
  int shot_pierce;
  fixed_t shot_speed;
  int shot_spread;
  int sight_range;
  int size;
  int turn_speed;

  int magnetism_dist;
  int shot_homing_power;
  int view_distance;
} GsPlayerStats;

typedef struct GsPlayer {
  fixed_t x;
  fixed_t y;
  fixed_t move_speed;
  angle_t angle;

  GsPlayerStats prev_stats;
  GsPlayerStats stats;
  int stat_update_timer;

  int hp;
  int ticks_since_damaged;

  int level;
  int xp;
  int total_xp;

  fixed_t reload_progress;
  fixed_t shot_progress;
  int ticks_since_last_shot;

  ItemType item_counts[ITEM_COUNT];
  int items_collected;

  int dps;
  int damage_history[60];
} GsPlayer;

typedef enum GsShapeVariant {
  SHAPE_VARIANT_NONE,
  SHAPE_VARIANT_BIG,
  SHAPE_VARIANT_FAST,
  SHAPE_VARIANT_HEALING,
  SHAPE_VARIANT_COUNT,
} GsShapeVariant;

typedef struct GsShape {
  bool exists;
  bool marked_for_despawn;
  bool grant_xp_on_despawn;
  bool spawn_children_on_despawn;
  int xp;

  uint8_t id;

  int sides;
  int size;
  Color fg;
  Color bg;
  GsShapeVariant variant;

  fixed_t x;
  fixed_t y;
  fixed_t max_move_speed;
  fixed_t move_speed;
  angle_t move_angle;

  fixed_t kb_speed;
  angle_t kb_angle;

  int sqdist_to_player;
  angle_t angle_to_player;
  int ticks_since_damaged_player;

  bool always_target;
  fixed_t target_x;
  fixed_t target_y;
  int sqdist_to_target;
  angle_t angle_to_target;

  int hp;
  int max_hp;
  int regen;
  int ticks_since_damaged;
  int i_frames;
} GsShape;

typedef struct GsProj {
  bool exists;

  fixed_t x;
  fixed_t y;
  fixed_t move_speed;
  angle_t angle;

  int damage;
  int pierce;
  int size;
  int despawn_timer;
  fixed_t kb;

  bool is_homing;
  int homing_max_dist;
  int homing_power;

  uint8_t hit_shape_ids[16];
} GsProj;

typedef struct GsPickup {
  bool exists;

  fixed_t x;
  fixed_t y;
  ItemType type;

  int sqdist_to_player;
  angle_t angle_to_player;
} GsPickup;

typedef struct GsTextFx {
  bool exists;

  fixed_t x;
  fixed_t y;
  int despawn_timer;
  char text[8];
} GsTextFx;

typedef struct GsXpOrb {
  bool exists;

  fixed_t x;
  fixed_t y;
  fixed_t move_speed;
  angle_t angle;

  int xp;
  int age;
  bool noticed_player;

} GsXpOrb;

typedef struct GsCamera {
  fixed_t x;
  fixed_t y;
  fixed_t zoom;
} GsCamera;

typedef struct GameScene {
  GsPlayer player;

  GsCamera camera;

  GsShape shapes[100];
  int shape_count;

  GsProj projs[60];

  GsPickup pickups[40];
  int pickups_spawned;

  GsTextFx text_fx[40];

  GsXpOrb xp_orbs[40];

  int ticks;
  Font font;

} GameScene;
