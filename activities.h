#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#include "execute_commands.h"

#define PROC_DIR "/proc"

extern ProcessInfo bg_processes[MAX_BG_PROCESSES];
extern int bg_process_count;

void activities();

#endif