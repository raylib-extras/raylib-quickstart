#include "gd.h"

const int render_w = 320;
const int render_h = 240;
const int default_window_scale = 3;
int window_scale = default_window_scale;
int window_w = render_w * default_window_scale;
int window_h = render_h * default_window_scale;

const int target_fps = 60;
const fixed_t default_z = 256;