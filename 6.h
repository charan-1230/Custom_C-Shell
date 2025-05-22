#ifndef EXECUTE_H
#define EXECUTE_H

#define MAX_BG_PROCESSES 4096
#define PATH_MAX 4096

// Struct to store process information
typedef struct {
    pid_t pid;
    char name[PATH_MAX];
} ProcessInfo;

extern pid_t foreground_pid;
extern ProcessInfo bg_processes[MAX_BG_PROCESSES];
extern int bg_process_count;
extern int restore_flag;
extern int input_fd;
extern int output_fd;

void foreground(char* command,char arg[20][PATH_MAX],int k);
void background(char* command,char arg[20][PATH_MAX],int k);

#endif