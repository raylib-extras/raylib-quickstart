#include "perf.h"

#include "gd.h"

PerfEntry perf_entries[20];

void PerfInit() {
}

void PerfAddData(int line, const char* name, int us) {
  for (int e = 0; e < LENGTHOF(perf_entries); ++e) {
    if (!perf_entries[e].exists) {
      perf_entries[e].exists = true;
      perf_entries[e].line = line;
      perf_entries[e].name = name;
    }

    // pointer comparison
    if (!strcmp(name, perf_entries[e].name)) {
      perf_entries[e].us_entries[perf_entries[e].us_index] = us;
      perf_entries[e].us_index = (perf_entries[e].us_index + 1) % LENGTHOF(perf_entries[e].us_entries);
      return;
    }
  }
  TraceLog(LOG_WARNING, "Ran out of perf entries!");
}

void PerfUninit() {
  FILE* f = fopen("perf_stats.txt", "w");
  if (f == NULL) {
    return;
  }

  for (int e = 0; e < LENGTHOF(perf_entries); ++e) {
    if (!perf_entries[e].exists) {
      continue;
    }
    fprintf(f, "%5d,%-20s,", perf_entries[e].line, perf_entries[e].name);
    for (int i = 0; i < LENGTHOF(perf_entries[e].us_entries); ++i) {
      int val = perf_entries[e].us_entries[i];
      fprintf(f, "%3d,", val);
    }
    fprintf(f, "\n");
  }

  int total_sum = 0;
  int total_min = INT_MAX;
  int total_max = INT_MIN;
  for (int e = 0; e < LENGTHOF(perf_entries); ++e) {
    if (!perf_entries[e].exists) {
      continue;
    }
    int sum = 0;
    int min = INT_MAX;
    int max = INT_MIN;
    for (int i = 0; i < LENGTHOF(perf_entries[e].us_entries); ++i) {
      int val = perf_entries[e].us_entries[i];
      if (val > 0) {
        sum += val;
        if (val < min) min = val;
        if (val > max) max = val;
        total_sum += val;
        if (val < total_min) total_min = val;
        if (val > total_max) total_max = val;
      }
    }
    double avg = (double)sum / LENGTHOF(perf_entries[e].us_entries);
    fprintf(f, "Line %5d | Name %-20s | Avg %8.2f us | Min %6d us | Max %6d us\n",
            perf_entries[e].line,
            perf_entries[e].name,
            avg,
            min,
            max);
  }

  double total_avg = (double)total_sum / LENGTHOF(perf_entries[0].us_entries);
  fprintf(f, "Line %5d | Name %-20s | Avg %8.2f us | Min %6d us | Max %6d us\n",
          0,
          "[total]",
          total_avg,
          total_min,
          total_max);

  fclose(f);
}

clock_t perf_start;
char perf_name[80] = "";

void PerfStart(const char* name) {
  strncpy(perf_name, name, LENGTHOF(perf_name));
  perf_start = clock();
}

void PerfEnd(const char* name) {
  clock_t end = clock();
  if (!strcmp(perf_name, name)) {
    PerfAddData(0, (name), end - perf_start);
  }
  strcpy(perf_name, "");
}
