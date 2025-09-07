#include "gs.h"

const char* stat_names[STAT_COUNT + 1] = {
    [STAT_MAX_HP] = "Max HP",
    [STAT_REGEN_DELAY] = "Regen Delay",
    [STAT_DAMAGE] = "Damage",
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
    [STAT_SHOT_HOMING_POWER] = "Homing Power",
    [STAT_VIEW_DISTANCE] = "View Distance",
    [STAT_COUNT] = "ERROR",
};

const bool stat_lower_is_better[STAT_COUNT] = {
    [STAT_REGEN_DELAY] = true,
    [STAT_RELOAD_DELAY] = true,
    [STAT_SHOT_SPREAD] = true,
};
