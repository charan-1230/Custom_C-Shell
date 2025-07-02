#include "headers.h"

void get_state(int pid, char *status)
{
    char pid_str[20];
    sprintf(pid_str, "%d", pid);
    char path[PATH_MAX];
    snprintf(path, sizeof(path), PROC_DIR "/%s/stat", pid_str);

    FILE *file = fopen(path, "r");
    if (!file)
    {
        fprintf(stderr, "\033[0;31m");
        perror("Failed to open stat file");
        fprintf(stderr, "\033[0m");
        return;
    }

    int unused;
    char state;
    fscanf(file, "%*d %*s %c %*d %*d", &state);
    fclose(file);

    *status = state;
}

int compare_process(const void *a, const void *b)
{
    ProcessInfo entryA = *(ProcessInfo *)a;
    ProcessInfo entryB = *(ProcessInfo *)b;
    return strcmp(entryA.name, entryB.name);
}

void activities()
{
    int no_of_process = bg_process_count;
    if (no_of_process == 0)
    {
        printf("Currently there are no processes spawn by the shell\n");
    }
    else
    {
        char status;
        ProcessInfo processes[no_of_process];
        for (int i = 0; i < no_of_process; i++)
        {
            processes[i].pid = bg_processes[i].pid;
            strcpy(processes[i].name, bg_processes[i].name);
        }
        qsort(processes, no_of_process, sizeof(ProcessInfo), compare_process);
        for (int i = 0; i < no_of_process; i++)
        {
            get_state(processes[i].pid, &status);
            if(status=='T')
            {
                printf("%d : %s - Stopped\n", processes[i].pid, processes[i].name);
            }
            else
            {
                printf("%d : %s - Running\n", processes[i].pid, processes[i].name);
            }
        }
    }
}
