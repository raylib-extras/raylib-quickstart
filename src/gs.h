#pragma once

#include <raylib.h>

#include "item_types.h"
#include "math_util.h"
#include "stddef.h"

typedef enum GsPlayerStatType {
  STAT_MAX_HP,
  STAT_ACTIVE_REGEN_DELAY,
  STAT_SHOT_DAMAGE,
  STAT_MAX_MOVE_SPEED,
  STAT_RELOAD_DELAY,
  STAT_SHOT_COUNT,
  STAT_SHOT_KB,
  STAT_SHOT_PIERCE,
  STAT_SHOT_SPEED,
  STAT_SHOT_SPREAD,
  STAT_SIGHT_RANGE,
  STAT_SIZE,
  STAT_TURN_SPEED,
  STAT_MAGNETISM_DIST,
  STAT_MAGNETISM_FREQUENCY,
  STAT_SHOT_HOMING_PERCENT,
  STAT_SHOT_HOMING_POWER,
  STAT_VIEW_DISTANCE,
  STAT_CONTACT_DAMAGE,
  STAT_ACTIVE_REGEN,
  STAT_PASSIVE_REGEN,
  STAT_CREATIVITY,
  STAT_SHOT_SPLIT_FRAGMENTS,
  STAT_SHOT_SPLIT_PERCENT,
  STAT_SHOT_FROST_PERCENT,
  STAT_MAX_ORBITALS,
  STAT_SPIKE_COUNT,
  STAT_SHOT_FLAME_PERCENT,
  STAT_SHOT_FLAME_POWER,
  STAT_SPLASH_DAMAGE,
  STAT_SPLASH_RADIUS,
  STAT_COUNT
} GsPlayerStatType;

// Access a stat statically by doing `stats.shot_damage` or dynamically with `stats.as_int[STAT_SHOT_DAMAGE]`.
typedef union GsPlayerStats {
  int as_int[STAT_COUNT];
  fixed_t as_fixed[STAT_COUNT];
  struct {
    int max_hp;
    int active_regen_delay;
    int shot_damage;
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
    int magnetism_frequency;
    int shot_homing_percent;
    int shot_homing_power;
    int view_distance;
    int contact_damage;
    int active_regen;
    int passive_regen;

    int creativity;

    int shot_split_fragments;
    int shot_split_percent;
    int shot_frost_percent;

    int max_orbitals;
    int max_spikes;
    int shot_flame_percent;
    int shot_flame_power;

    int splash_damage;
    int splash_radius;
  };
} GsPlayerStats;

extern const char* stat_names[STAT_COUNT + 1];
extern const bool stat_lower_is_better[STAT_COUNT];

typedef struct GsPlayer {
  fixed_t x;
  fixed_t y;
  fixed_t move_speed;
  angle_t angle;

  GsPlayerStats stats;
  GsPlayerStats tmp_stats;

  int hp;
  int ticks_since_damaged;

  int level;
  int upgrades_pending;
  int xp;
  int total_xp;

  fixed_t reload_progress;
  fixed_t shot_progress;
  int ticks_since_last_shot;

  int ticks_since_magnetism;

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
  int contact_damage;

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

  int frost_ticks;
  int flame_ticks;

} GsShape;

typedef struct GsProjOrbit {
  int timer;  // 0 means inactive

  fixed_t cx;  // center x, center y
  fixed_t cy;

  int radius;
  int target_radius;
  angle_t angle;

  int rot_speed;

  bool track_player_angle;
  angle_t track_player_angle_offset;

} GsProjOrbit;

typedef struct GsProj {
  bool exists;

  fixed_t x;
  fixed_t y;
  fixed_t move_speed;
  angle_t move_angle;

  GsProjOrbit orbit;

  int damage;
  int pierce;
  int size;
  int despawn_timer;
  fixed_t kb;

  bool is_homing;
  int homing_max_dist;
  int homing_power;

  int frost_power;

  int split_fragments;  // 0 for non-splitting shot

  bool is_spike;

  int flame_power;

  int splash_damage;
  int splash_radius;

  int ticks_since_last_hit;

  uint8_t hit_shape_ids[16];
  uint8_t hit_shape_timers[16];
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

  Color color;
} GsTextFx;

typedef struct GsLineFx {
  bool exists;

  fixed_t x1;
  fixed_t y1;

  fixed_t x2;
  fixed_t y2;
  int despawn_timer;

  Color color;
} GsLineFx;

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

typedef enum GsOverlayType {
  GS_OVERLAY_NONE,
  GS_OVERLAY_PICK_ITEM,
  GS_OVERLAY_STATS,
  GS_OVERLAY_ITEMS,
} GsOverlayType;

typedef struct GsOlPickItemChoice {
  ItemType item_a;
  ItemType item_b;
  ItemType removed_item;

} GsOlPickItemChoice;

typedef struct GsOlPickItem {
  int item_count;

  GsOlPickItemChoice choices[8];

  int selected_item_idx;
  int ticks;
} GsOlPickItem;

typedef struct GsDrawData {
  fixed_t xp_bar_filled_width;
} GsDrawData;

typedef struct GameScene {
  GsPlayer player;

  GsCamera camera;

  GsShape shapes[200];
  int shape_count;

  GsProj projs[200];
  int orbital_proj_count;
  int spike_proj_count;

  GsPickup pickups[40];
  int pickups_spawned;

  GsTextFx text_fx[40];
  GsLineFx line_fx[40];

  GsXpOrb xp_orbs[80];

  GsOverlayType curr_overlay;
  GsOlPickItem ol_pick_item;

  GsDrawData draw_data;

  int ticks;

} GameScene;
