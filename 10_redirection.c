#include "headers.h"

extern char history_path[100];

void execute_command(char *command, char arg[20][4096], int k, char *home_dir, char *prev_dir)
{
    if (strcmp(command, "hop") == 0)
        hop(arg, k, home_dir, prev_dir);
    else if (strcmp(command, "reveal") == 0)
        reveal(arg, k, home_dir, prev_dir);
    else if (strcmp(command, "log") == 0)
    {
        if (k == 0)
        {
            FILE *file = fopen(history_path, "r");
            if (!file)
                printf("\033[0;31mError:There is no history stored,This is your first command!\033[0m\n");
            else
            {
                char line[PATH_MAX] = "";
                if (!fgets(line, PATH_MAX, file))
                {
                    printf("\033[0;31moutput is nothing since log has been purged\033[0m\n");
                    fclose(file);
                }
                else
                {
                    fclose(file);
                    file = fopen(history_path, "r");
                    while (fgets(line, PATH_MAX, file))
                    {
                        printf("%s", line);
                    }
                    fclose(file);
                }
            }
        }
        else if (k == 1)
        {
            if (strcmp(arg[0], "purge") == 0)
            {
                FILE *file = fopen(history_path, "w");
                fclose(file);
            }
            else if (strcmp(arg[0], "execute") == 0)
                printf("\033[0;31mError : Please provide index\033[0m\n");
            else
                printf("\033[0;31mError : invalid argument\033[0m\n");
        }
        else if (k == 2)
        {
            if (is_all_digits(arg[1]))
            {
                FILE *file = fopen(history_path, "r");
                if (!file)
                    printf("\033[0;31mError:There is no history stored,This is your first command!\033[0m\n");
                else
                {
                    int log_history_cnt = 0;
                    char line[PATH_MAX] = "";
                    while (fgets(line, PATH_MAX, file))
                    {
                        log_history_cnt++;
                    }
                    fclose(file);
                    if (log_history_cnt != 0)
                    {
                        file = fopen(history_path, "r");
                        int index = 0;
                        index = atoi(arg[1]);
                        if (index > log_history_cnt)
                        {
                            printf("\033[0;31mAt present log history doesn't contain %d commands!\033[0m\n", index);
                        }
                        else
                        {
                            for (int i = 0; i < (log_history_cnt - index); i++)
                            {
                                fgets(line, PATH_MAX, file);
                            }
                            fgets(line, PATH_MAX, file);
                            line[strlen(line) - 1] = '\0';
                            log_execute(line, home_dir, prev_dir);
                        }
                    }
                    else
                    {
                        printf("\033[0;31moutput is nothing since log has been purged\033[0m\n");
                    }
                }
            }
            else
            {
                printf("\033[0;31mError : provide a valid index\033[0m\n");
            }
        }
    }
    else if (strcmp(command, "proclore") == 0)
    {
        if (k == 0)
        {
            __pid_t pid = getpid();
            char pid_str[PATH_MAX] = "";
            sprintf(pid_str, "%d", pid);
            print_proc_info(pid_str);
        }
        else if (k == 1)
        {
            print_proc_info(arg[0]);
        }
    }
    else if (strcmp(command, "seek") == 0)
    {
        seek(arg, k, home_dir, prev_dir);
    }

    else if (strcmp(command, "exit") == 0)
    {
        free(prev_dir);
        kill_all_processes();
        exit(0);
    }
    else if (strcmp(command, "activities") == 0)
    {
        if (k == 0)
            activities();
        else
        {
            printf(RED);
            printf("Error:please provide a valid command\n");
            printf(RESET);
        }
    }
    else if (strcmp(command, "ping") == 0)
    {
        execute_ping(arg, k);
    }
    else if (strcmp(command, "fg") == 0)
    {
        fg(arg, k);
    }
    else if (strcmp(command, "bg") == 0)
    {
        bg(arg, k);
    }
    else if (strcmp(command, "neonate") == 0)
    {
        neonate(arg, k);
    }
    else if (strcmp(command, "iMan") == 0)
    {
        if (k == 0)
        {
            printf(RED);
            printf("Error:please provide details of which man page do you want?\n");
            printf(RESET);
        }
        else if (k >= 1)
        {
            man_page(command, arg);
        }
    }
    else if (strcmp(command, "") != 0)
    {
        foreground(command, arg, k);
    }
}

int redirect_stdin(int *save_stdin, char *input_file)
{
    (*save_stdin) = dup(0);
    if ((*save_stdin) == -1)
    {
        printf(RED);
        printf("Error while duplicating stdin\n");
        printf(RESET);
        return 0;
    }
    (input_fd) = open(input_file, O_RDONLY);
    if ((input_fd) == -1)
    {
        printf(RED);
        printf("Error: No such input file found!\n");
        printf(RESET);
        return 0;
    }
    if (dup2(input_fd, 0) == -1)
    {
        printf(RED);
        printf("Error while redirecting stdin\n");
        printf(RESET);
        close(input_fd);
        input_fd = -3;
        return 0;
    }
    return 1;
}

int redirect_stdout(int *save_stdout, char *output_file, int mode)
{
    (*save_stdout) = dup(1);
    if ((*save_stdout) == -1)
    {
        printf(RED);
        printf("Error while duplicating stdout\n");
        printf(RESET);
        return 0;
    }
    if (mode == 0)
    {
        output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }
    else if (mode == 1)
    {
        output_fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    }
    if (output_fd == -1)
    {
        printf(RED);
        printf("Error: unable to open input file\n");
        printf(RESET);
        close((*save_stdout));
        return 0;
    }
    if (dup2(output_fd, 1) == -1)
    {
        printf(RED);
        printf("Error while redirecting stdout\n");
        printf(RESET);
        close(output_fd);
        output_fd = -4;
        close((*save_stdout));
        return 0;
    }
    return 1;
}

void redirect_in_fg(char *command, char arg[20][4096], char *input_file, int k, char *home_dir, char *prev_dir)
{
    int save_stdin;
    if (redirect_stdin(&save_stdin, input_file))
    {
        execute_command(command, arg, k, home_dir, prev_dir);
        close(input_fd);
        input_fd = -3;
        if (dup2(save_stdin, 0) == -1)
        {
            printf(RED);
            printf("Error while restoring stdin\n");
            printf(RESET);
        }
        close(save_stdin);
    }
}

void redirect_in_bg(char *command, char arg[20][4096], char *input_file, int k, char *home_dir, char *prev_dir)
{
    int save_stdin;
    if (redirect_stdin(&save_stdin, input_file))
    {
        if (strcmp(command, "") != 0)
        {
            restore_flag = 0;
            background(command, arg, k);
        }
        else
        {
            close(input_fd);
            input_fd = -3;
            if (dup2(save_stdin, 0) == -1)
            {
                printf(RED);
                printf("Error while restoring stdin\n");
                printf(RESET);
            }
            close(save_stdin);
        }
    }
}

void redirect_out_fg(char *command, char arg[20][4096], char *output_file, int k, char *home_dir, char *prev_dir)
{
    int save_stdout;
    if (redirect_stdout(&save_stdout, output_file, 0))
    {
        execute_command(command, arg, k, home_dir, prev_dir);
        fflush(stdout);

        close(output_fd);
        output_fd = -4;
        if (dup2(save_stdout, 1) == -1)
        {
            printf(RED);
            printf("Error while restoring stdout\n");
            printf(RESET);
        }
        close(save_stdout);  
    }
}

void redirect_out_bg(char *command, char arg[20][4096], char *output_file, int k, char *home_dir, char *prev_dir)
{
    int save_stdout;
    if (redirect_stdout(&save_stdout, output_file, 0))
    {
        if (strcmp(command, "") != 0)
        {
            restore_flag = 1;
            background(command, arg, k);
        }
        else
        {
            close(output_fd);
            output_fd = -4;
            if (dup2(save_stdout, 1) == -1)
            {
                printf(RED);
                printf("Error while restoring stdout\n");
                printf(RESET);
            }
            close(save_stdout);
        }
    }
}
void redirect_out2_fg(char *command, char arg[20][4096], char *output_file, int k, char *home_dir, char *prev_dir)
{
    int save_stdout;
    if (redirect_stdout(&save_stdout, output_file, 1))
    {
        execute_command(command, arg, k, home_dir, prev_dir);
        close(output_fd);
        output_fd = -4;
        if (dup2(save_stdout, 1) == -1)
        {
            printf(RED);
            printf("Error while restoring stdout\n");
            printf(RESET);
        }
        close(save_stdout);
    }
}

void redirect_out2_bg(char *command, char arg[20][4096], char *output_file, int k, char *home_dir, char *prev_dir)
{
    int save_stdout;
    if (redirect_stdout(&save_stdout, output_file, 1))
    {
        if (strcmp(command, "") != 0)
        {
            restore_flag = 1;
            background(command, arg, k);
        }
        else
        {
            close(output_fd);
            output_fd = -4;
            if (dup2(save_stdout, 1) == -1)
            {
                printf(RED);
                printf("Error while restoring stdout\n");
                printf(RESET);
            }
            close(save_stdout);
        }
    }
}
void redirect_inout_fg(char *command, char arg[20][4096], char *input_file, char *output_file, int k, char *home_dir, char *prev_dir)
{
    int save_stdin, save_stdout;
    if (redirect_stdin(&save_stdin, input_file) && redirect_stdout(&save_stdout, output_file, 0))
    {
        execute_command(command, arg, k, home_dir, prev_dir);
        close(input_fd);
        input_fd = -3;
        close(output_fd);
        output_fd = -4;
        if (dup2(save_stdin, 0) == -1)
        {
            printf(RED);
            printf("Error while restoring stdin\n");
            printf(RESET);
        }
        close(save_stdin);
        if (dup2(save_stdout, 1) == -1)
        {
            printf(RED);
            printf("Error while restoring stdout\n");
            printf(RESET);
        }
        close(save_stdout);
    }
}
void redirect_inout_bg(char *command, char arg[20][4096], char *input_file, char *output_file, int k, char *home_dir, char *prev_dir)
{
    int save_stdin, save_stdout;
    if (redirect_stdin(&save_stdin, input_file) && redirect_stdout(&save_stdout, output_file, 0))
    {
        if (strcmp(command, "") != 0)
        {
            restore_flag = 2;
            background(command, arg, k);
        }
        else
        {
            close(input_fd);
            input_fd = -3;
            close(output_fd);
            output_fd = -4;
            if (dup2(save_stdin, 0) == -1)
            {
                printf(RED);
                printf("Error while restoring stdin\n");
                printf(RESET);
            }
            close(save_stdin);
            if (dup2(save_stdout, 1) == -1)
            {
                printf(RED);
                printf("Error while restoring stdout\n");
                printf(RESET);
            }
            close(save_stdout);
        }
    }
}
void redirect_inout2_fg(char *command, char arg[20][4096], char *input_file, char *output_file, int k, char *home_dir, char *prev_dir)
{
    int save_stdin, save_stdout;
    if (redirect_stdin(&save_stdin, input_file) && redirect_stdout(&save_stdout, output_file, 1))
    {
        execute_command(command, arg, k, home_dir, prev_dir);
        close(input_fd);
        input_fd = -3;
        close(output_fd);
        output_fd = -4;
        if (dup2(save_stdin, 0) == -1)
        {
            printf(RED);
            printf("Error while restoring stdin\n");
            printf(RESET);
        }
        close(save_stdin);
        if (dup2(save_stdout, 1) == -1)
        {
            printf(RED);
            printf("Error while restoring stdout\n");
            printf(RESET);
        }
        close(save_stdout);
    }
}
void redirect_inout2_bg(char *command, char arg[20][4096], char *input_file, char *output_file, int k, char *home_dir, char *prev_dir)
{
    int save_stdin, save_stdout;
    if (redirect_stdin(&save_stdin, input_file) && redirect_stdout(&save_stdout, output_file, 1))
    {
        if (strcmp(command, "") != 0)
        {
            restore_flag = 2;
            background(command, arg, k);
        }
        else
        {
            close(input_fd);
            input_fd = -3;
            close(output_fd);
            output_fd = -4;
            if (dup2(save_stdin, 0) == -1)
            {
                printf(RED);
                printf("Error while restoring stdin\n");
                printf(RESET);
            }
            close(save_stdin);
            if (dup2(save_stdout, 1) == -1)
            {
                printf(RED);
                printf("Error while restoring stdout\n");
                printf(RESET);
            }
            close(save_stdout);
        }
    }
}
