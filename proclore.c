#include "headers.h"

// Function to check if a directory name is a process ID
int check_pid_dir(const char *dir_name)
{
    while (*dir_name)
    {
        if (!isdigit(*dir_name))
        {
            return 0;
        }
        dir_name++;
    }
    return 1;
}

void get_status_and_group(const char *pid, char *status, int *pgrp, int *is_foreground)
{
    char path[PATH_MAX];
    snprintf(path, sizeof(path), PROC_DIR "/%s/stat", pid);

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
    fscanf(file, "%d %*s %c %*d %d", &unused, &state, pgrp);
    fclose(file);

    *status = state;

    // Check if the process group is in the foreground
    *is_foreground = 0;
    int fd = open("/dev/tty", O_RDONLY);
    if (fd != -1) {
        int fg_pgrp = tcgetpgrp(fd);
        if (fg_pgrp != -1 && fg_pgrp == *pgrp) {
            *is_foreground = 1;
        }
        close(fd);
    }

}

long get_virtual_memory(const char *pid)
{
    char path[PATH_MAX];
    snprintf(path, sizeof(path), PROC_DIR "/%s/statm", pid);

    FILE *file = fopen(path, "r");
    if (!file)
    {
        fprintf(stderr, "\033[0;31m");
        perror("Failed to open statm file");
        fprintf(stderr, "\033[0m");
        return -1;
    }

    long vsize;
    fscanf(file, "%ld", &vsize);
    fclose(file);
    return vsize;
}

void get_executable_path(const char *pid, char *exe_path, size_t size)
{
    char path[PATH_MAX];
    snprintf(path, sizeof(path), PROC_DIR "/%s/exe", pid);

    ssize_t len = readlink(path, exe_path, size - 1);
    if (len != -1)
    {
        exe_path[len] = '\0';
    }
    else
    {
        fprintf(stderr, "\033[0;31m");
        perror("Failed to read exe path");
        fprintf(stderr, "\033[0m");
        strcpy(exe_path, "Not Accessible");
    }
}

void print_proc_info(char *pid)
{
    if (!check_pid_dir(pid))
    {
        printf("\033[0;31mError: please provide a valid pid number\033[0m\n");
    }
    else
    {
        char status;
        int pgrp;
        int is_foreground;

        get_status_and_group(pid, &status, &pgrp, &is_foreground);

        char status_str[4];
        snprintf(status_str, sizeof(status_str), "%c", status);
        if (is_foreground)
        {
            snprintf(status_str, sizeof(status_str), "%c+", status);
        }

        long vsize = get_virtual_memory(pid);

        char exe_path[PATH_MAX];
        get_executable_path(pid, exe_path, sizeof(exe_path));

        printf("PID : %s\n", pid);
        printf("Process Status : %s\n", status_str);
        printf("Process Group : %d\n", pgrp);
        printf("Virtual Memory : %ld\n", vsize);
        printf("Executable Path : %s\n", exe_path);
    }
}
