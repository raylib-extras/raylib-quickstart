#pragma once

#include <stdbool.h>
#include <string.h>
#include <time.h>

typedef struct PerfEntry {
  bool exists;
  int line;
  const char* name;
  int us_index;
  int us_entries[120];

} PerfEntry;

extern PerfEntry perf_entries[20];

void PerfInit();
void PerfAddData(int line, const char* name, int us);
void PerfUninit();

void PerfStart(const char* name);
void PerfEnd(const char* name);

#define PERF_EXPR(name, code) \
  do {                        \
    PerfStart(name);          \
    (code);                   \
    PerfEnd(name);            \
  } while (0);
