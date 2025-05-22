#ifndef SIGNALS_H
#define SIGNALS_H

extern pid_t foreground_pid;
extern ProcessInfo bg_processes[MAX_BG_PROCESSES];
extern int bg_process_count;

void kill_all_processes();
void handle_sigchld(int sig);
void handle_sigint(int sig);
void handle_sigtstp(int sig);
void execute_ping(char arg[20][PATH_MAX], int k);

#endif