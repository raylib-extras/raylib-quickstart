#include "gs.h"

const char* stat_names[STAT_COUNT + 1] = {
    [STAT_MAX_HP] = "Max HP",
    [STAT_ACTIVE_REGEN_DELAY] = "Regen Delay",  // Active Regen Delay
    [STAT_SHOT_DAMAGE] = "Shot Damage",
    [STAT_MAX_MOVE_SPEED] = "Move Speed",
    [STAT_RELOAD_DELAY] = "Reload Delay",
    [STAT_SHOT_COUNT] = "Shot Count",
    [STAT_SHOT_KB] = "Shot Knockback",
    [STAT_SHOT_PIERCE] = "Shot Pierce",
    [STAT_SHOT_SPEED] = "Shot Speed",
    [STAT_SHOT_SPREAD] = "Shot Spread",
    [STAT_SIGHT_RANGE] = "Sight Range",
    [STAT_SIZE] = "Size",
    [STAT_TURN_SPEED] = "Turn Speed",
    [STAT_MAGNETISM_DIST] = "Magnetism Power",
    [STAT_MAGNETISM_FREQUENCY] = "Magnetism Delay",
    [STAT_SHOT_HOMING_PERCENT] = "Homing %",
    [STAT_SHOT_HOMING_POWER] = "Homing Power",
    [STAT_VIEW_DISTANCE] = "View Distance",
    [STAT_CONTACT_DAMAGE] = "Contact Damage",
    [STAT_ACTIVE_REGEN] = "Active Regen",
    [STAT_PASSIVE_REGEN] = "Passive Regen",
    [STAT_CREATIVITY] = "Creativity",
    [STAT_SHOT_SPLIT_FRAGMENTS] = "Split Fragments",
    [STAT_SHOT_SPLIT_PERCENT] = "Split %",
    [STAT_SHOT_FROST_PERCENT] = "Frost %",
    [STAT_MAX_ORBITALS] = "Orbital Count",
    [STAT_SPIKE_COUNT] = "Spike Count",
    [STAT_SHOT_FLAME_PERCENT] = "Flame %",
    [STAT_SHOT_FLAME_POWER] = "Flame Damage",
    [STAT_SPLASH_DAMAGE] = "Splash Damage",
    [STAT_SPLASH_RADIUS] = "Splash Radius",
    [STAT_COUNT] = "ERROR",
};

const bool stat_lower_is_better[STAT_COUNT] = {
    [STAT_ACTIVE_REGEN_DELAY] = true,
    [STAT_RELOAD_DELAY] = true,
    [STAT_SHOT_SPREAD] = true,
    [STAT_MAGNETISM_FREQUENCY] = true,
};
