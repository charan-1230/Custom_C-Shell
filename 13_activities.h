#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#include "6.h"

#define PROC_DIR "/proc"

extern ProcessInfo bg_processes[MAX_BG_PROCESSES];
extern int bg_process_count;

void activities();

#endif