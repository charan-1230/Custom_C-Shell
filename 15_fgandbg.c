#include "headers.h"

void get_terminal_pgid(int *t_pgid)
{
    int fd = open("/dev/tty", O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    *t_pgid = tcgetpgrp(fd);
    if ((*t_pgid) == -1) {
        perror("tcgetpgrp");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
}

void fg(char arg[20][PATH_MAX], int k)
{
    if (k == 0)
    {
        printf(RED);
        printf("Error:Please provide pid\n");
        printf(RESET);
    }
    else if (k == 1)
    {
        if (is_all_digits(arg[0]))
        {
            int status;
            int t_pgid;
            int flag = 0;
            char command_name[PATH_MAX] = "";
            pid_t pid = atoi(arg[0]);
            for (int i = 0; i < bg_process_count; i++)
            {
                if (bg_processes[i].pid == pid)
                {
                    flag = 1;
                    strcpy(command_name, bg_processes[i].name);
                    if (kill(pid, SIGCONT) == -1)
                    {
                        fprintf(stderr, RED);
                        fprintf(stderr, "Error sending signal: %s\n", strerror(errno));
                        fprintf(stderr, RESET);
                        return;
                    }
                    bg_processes[i] = bg_processes[--bg_process_count];
                    foreground_pid = pid;
                    get_terminal_pgid(&t_pgid);
                    setpgid(foreground_pid,t_pgid);
                    printf("bringing bg_process with pid %d to foreground \n", pid);
                    break;
                }
            }
            if (flag == 1)
            {
                waitpid(foreground_pid, &status, WUNTRACED);
                if (WIFSTOPPED(status))
                {
                    printf("Process %d was stopped by signal %d\n", foreground_pid, WSTOPSIG(status));
                    bg_processes[bg_process_count].pid = foreground_pid;
                    strcpy(bg_processes[bg_process_count].name, command_name);
                    bg_process_count++;
                }
            }
            else
            {
                printf(RED);
                printf("No such process found\n");
                printf(RESET);
            }
            foreground_pid = -2;
            return;
        }
        else
        {
            printf(RED);
            printf("Error:Please provide valid pid\n");
            printf(RESET);
        }
    }
    else
    {
        printf(RED);
        printf("Error:Too many arguments!\n");
        printf(RESET);
    }
}

void bg(char arg[20][PATH_MAX], int k)
{
    if (k == 0)
    {
        printf(RED);
        printf("Error:Please provide pid\n");
        printf(RESET);
    }
    else if (k == 1)
    {
        if (is_all_digits(arg[0]))
        {
            int flag = 0;
            pid_t pid = atoi(arg[0]);
            for (int i = 0; i < bg_process_count; i++)
            {
                if (bg_processes[i].pid == pid)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {
                if (kill(pid, SIGCONT) == -1)
                {
                    fprintf(stderr, RED);
                    fprintf(stderr, "Error sending signal: %s\n", strerror(errno));
                    fprintf(stderr, RESET);
                    return;
                }
                printf("Sent SIGCONT to process with pid %d\n", pid);
            }
            else
            {
                printf(RED);
                printf("No such process found\n");
                printf(RESET);
            }
        }
        else
        {
            printf(RED);
            printf("Error:Please provide valid pid\n");
            printf(RESET);
        }
    }
    else
    {
        printf(RED);
        printf("Error:Too many arguments!\n");
        printf(RESET);
    }
}
