#include "headers.h"

void foreground(char *command, char arg[20][PATH_MAX], int k)
{
    int child_pid = fork();
    if (child_pid < 0)
    {
        fprintf(stderr, "\033[0;31m");
        perror("fork1\n");
        fprintf(stderr, "\033[0m");
    }
    else if (child_pid == 0)
    {
        char *arg_exec[k + 2]; // +2 for command and NULL termination
        arg_exec[0] = command;
        char temp[k][PATH_MAX];
        int iter;
        if (strcmp(command, "echo") == 0)
        {
            for (int i = 0; i < k; i++)
            {
                iter = 0;
                int x = strlen(arg[i]);
                for (int j = 0; j < x; j++)
                {
                    if (arg[i][j] != '"')
                        temp[i][iter++] = arg[i][j];
                }
                temp[i][iter] = '\0';
            }
            for (int i = 0; i < k; i++)
            {
                arg_exec[i + 1] = temp[i];
            }
        }
        else
        {
            for (int i = 0; i < k; i++)
            {
                arg_exec[i + 1] = arg[i];
            }
        }
        // NULL terminate the array
        arg_exec[k + 1] = NULL;

        if (execvp(arg_exec[0], arg_exec) == -1)
        {
            printf("\033[0;31mError : %s ", command);
            for (int i = 0; i < k - 1; i++)
            {
                printf("%s ", arg[i]);
            }
            printf("%s is not valid command \033[0m\n", arg[k - 1]);
            exit(EXIT_FAILURE);
        }
        exit(0);
    }
    else
    {
        int status;
        foreground_pid = child_pid;
        setpgid(foreground_pid, 0);
        waitpid(foreground_pid, &status, WUNTRACED);
        if (WIFSTOPPED(status))
        {
            printf("Process %d was stopped by signal %d\n", foreground_pid, WSTOPSIG(status));
            bg_processes[bg_process_count].pid = child_pid;
            strcpy(bg_processes[bg_process_count].name, command);
            bg_process_count++;
        }
        foreground_pid = -2;
    }
    return;
}

void background(char *command, char arg[20][PATH_MAX], int k)
{
    int child_pid = fork();
    if (child_pid < 0)
    {
        fprintf(stderr, "\033[0;31m");
        perror("fork1\n");
        fprintf(stderr, "\033[0m");
    }
    else if (child_pid == 0)
    {
        setpgid(0, 0);
        char *arg_exec[k + 2]; // +2 for command and NULL termination
        arg_exec[0] = command;
        char temp[k][PATH_MAX];
        int iter;
        if (strcmp(command, "echo") == 0)
        {
            for (int i = 0; i < k; i++)
            {
                iter = 0;
                int x = strlen(arg[i]);
                for (int j = 0; j < x; j++)
                {
                    if (arg[i][j] != '"')
                        temp[i][iter++] = arg[i][j];
                }
                temp[i][iter] = '\0';
            }
            for (int i = 0; i < k; i++)
            {
                arg_exec[i + 1] = temp[i];
            }
        }
        else
        {
            for (int i = 0; i < k; i++)
            {
                arg_exec[i + 1] = arg[i];
            }
        }
        // NULL terminate the array
        arg_exec[k + 1] = NULL;

        if (execvp(arg_exec[0], arg_exec) == -1)
        {
            printf("\033[0;31mError : %s ", command);
            for (int i = 0; i < k - 1; i++)
            {
                printf("%s ", arg[i]);
            }
            printf("%s is not valid command in background\033[0m\n", arg[k - 1]);
            exit(EXIT_FAILURE);
        }
        exit(0);
    }
    else
    {
        if (restore_flag == 0)
        {
            close(input_fd);
            input_fd=-3;
            if (freopen("/dev/tty", "r", stdin) == NULL)
            {
                fprintf(stderr, RED);
                perror(" error at freopen to /dev/tty for stdin");
                fprintf(stderr, RESET);
                return;
            }
            restore_flag=-1;
        }
        else if (restore_flag == 1)
        {
            close(output_fd);
            output_fd=-4;
            if (freopen("/dev/tty", "w", stdout) == NULL)
            {
                fprintf(stderr, RED);
                perror(" error at freopen to /dev/tty for stdout");
                fprintf(stderr, RESET);
                return;
            }
            restore_flag=-1;
        }
        else if (restore_flag == 2)
        {
            close(input_fd);
            close(output_fd);
            input_fd=-3;
            output_fd=-4;
            if (freopen("/dev/tty", "r", stdin) == NULL)
            {
                fprintf(stderr, RED);
                perror(" error at freopen to /dev/tty for stdin");
                fprintf(stderr, RESET);
                return;
            }
            if (freopen("/dev/tty", "w", stdout) == NULL)
            {
                fprintf(stderr, RED);
                perror(" error at freopen to /dev/tty for stdout");
                fprintf(stderr, RESET);
                return;
            }
            restore_flag=-1;
        }

        bg_processes[bg_process_count].pid = child_pid;
        strcpy(bg_processes[bg_process_count].name, command);
        bg_process_count++;
        printf("[%d] %d\n", bg_process_count, child_pid);
    }
    return;
}
