#include "headers.h"

void handle_sigchld(int sig)
{
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        for (int i = 0; i < bg_process_count; i++)
        {
            if (bg_processes[i].pid == pid)
            {
                if (WIFEXITED(status))
                {
                    printf("\033[0;33m%s\033[0m exited normally \033[0;33m(PID: %d)\033[0m\n", bg_processes[i].name, pid);
                    fflush(stdout);
                }
                else if (WIFSIGNALED(status))
                {
                    printf("\033[0;33m%s\033[0m exited abnormally \033[0;33m(PID: %d)\033[0m\n", bg_processes[i].name, pid);
                    fflush(stdout);
                }
                bg_processes[i] = bg_processes[--bg_process_count];
                break;
            }
        }
    }
}

void handle_sigint(int sig)
{
    if (foreground_pid != -2)
    {
        if (kill(foreground_pid, SIGINT) == -1)
        {
            fprintf(stderr, RED);
            perror("Error sending SIGINT to foreground process");
            fprintf(stderr, RESET);
        }
        else
        {
            printf("\n");
        }
    }
}

void handle_sigtstp(int sig)
{
    if (foreground_pid != -2)
    {
        if (kill(foreground_pid, SIGTSTP) == -1)
        {
            fprintf(stderr, RED);
            perror("Error sending SIGTSTP to foreground process");
            fprintf(stderr, RESET);
        }
        else
        {
            printf("\n");
        }
    }
}

void kill_all_processes()
{
    int cnt = bg_process_count;
    int bg_pids[cnt];
    for (int i = 0; i < cnt; i++)
    {
        bg_pids[i] = bg_processes[i].pid;
    }
    for (int i = 0; i < cnt; i++)
    {
        if (kill(bg_pids[i], SIGKILL) == -1)
        {
            if (errno == ESRCH)
            {
                printf(RED);
                printf("No such process found\n");
                printf(RESET);
            }
            else
            {
                fprintf(stderr, RED);
                fprintf(stderr, "Error sending signal: %s\n", strerror(errno));
                fprintf(stderr, RESET);
            }
            return;
        }
    }
    printf("\n");
}
void execute_ping(char arg[20][PATH_MAX], int k)
{

    if (k == 0)
    {
        printf(RED);
        printf("Error:Please provide pid and signal_number\n");
        printf(RESET);
    }
    else if (k == 1)
    {
        if (is_all_digits(arg[0]))
        {
            printf(RED);
            printf("Error:Please provide signal_number\n");
            printf(RESET);
        }
        else
        {
            printf(RED);
            printf("Error:Please provide valid pid and signal_number\n");
            printf(RESET);
        }
    }
    else if (k == 2)
    {
        int valid_pid = is_all_digits(arg[0]);
        int valid_signal_number = is_all_digits(arg[1]);
        if (valid_pid && valid_signal_number)
        {
            int flag = 0;
            pid_t pid = atoi(arg[0]);
            for (int i = 0; i < bg_process_count; i++)
            {
                if (bg_processes[i].pid == pid)
                    flag = 1;
            }
            if (flag == 0)
            {
                printf(RED);
                printf("No such process found\n");
                printf(RESET);
                return;
            }
            int signal_number = atoi(arg[1]);
            signal_number = signal_number % 32;
            if (kill(pid, signal_number) == -1)
            {
                fprintf(stderr, RED);
                fprintf(stderr, "Error sending signal: %s\n", strerror(errno));
                fprintf(stderr, RESET);
                return;
            }
            printf("Sent signal %d to process with pid %d\n", signal_number, pid);
            return;
        }
        else
        {
            if (!valid_pid)
            {
                if (!valid_signal_number)
                {
                    printf(RED);
                    printf("Error:Please provide valid pid and valid signal number\n");
                    printf(RESET);
                }
                else
                {
                    printf(RED);
                    printf("Error:Please provide valid pid\n");
                    printf(RESET);
                }
            }
            else if (!valid_signal_number)
            {
                printf(RED);
                printf("Error:Please provide valid signal_number\n");
                printf(RESET);
            }
        }
    }
    else
    {
        printf(RED);
        printf("Error:Too many arguments!\n");
        printf(RESET);
    }
}
