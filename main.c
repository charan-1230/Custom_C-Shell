#include "headers.h"

char history_path[100];
time_t start;
char user_prompt[PATH_MAX];
int flag_first_prompt;

ProcessInfo bg_processes[MAX_BG_PROCESSES];
int bg_process_count = 0;

pid_t foreground_pid = -2;
int restore_flag = -1;
int input_fd = -3;
int output_fd = -4;

int main()
{
    // signal handler for SIGCHLD
    signal(SIGCHLD, handle_sigchld);

    // signal handler for SIGINT (Ctrl + C)
    signal(SIGINT, handle_sigint);

    // Signal handler for SIGTSTP (Ctrl + Z)
    signal(SIGTSTP, handle_sigtstp);

    flag_first_prompt = 1;

    // get the home directory of the shell
    char home_dir[PATH_MAX];
    if (getcwd(home_dir, sizeof(home_dir)) == NULL)
    {
        fprintf(stderr, "\033[0;31m");
        perror("getcwd() error");
        fprintf(stderr, "\033[0m");
        return 1;
    }

    strcpy(history_path, home_dir);
    strcat(history_path, "/");
    strcat(history_path, "history.txt");

    char *prev_dir = (char *)malloc(PATH_MAX * sizeof(char));
    if (prev_dir == NULL)
    {
        fprintf(stderr, "\033[0;31m");
        perror("malloc failed");
        fprintf(stderr, "\033[0m");
        return 0;
    }
    strcpy(prev_dir, "\0");

    int error_display;

    while (1)
    {
        // get the current working directory of the shell
        char current_dir[PATH_MAX] = "";
        if (getcwd(current_dir, sizeof(current_dir)) == NULL)
        {
            fprintf(stderr, "\033[0;31m");
            perror("getcwd() error");
            fprintf(stderr, "\033[0m");
            return 1;
        }

        //  display the shell prompt
        error_display = display(home_dir, current_dir);
        if (error_display == 1)
            return 0;

        if (fgets(user_prompt, sizeof(user_prompt), stdin) != NULL)
        {
            size_t len = strlen(user_prompt);
            if (len > 0 && user_prompt[len - 1] == '\n')
            {
                user_prompt[len - 1] = '\0';
            }
            time(&start);
            if (strcmp(user_prompt, "") == 0)
                continue;
        }
        else
        {
            free(prev_dir);
            kill_all_processes();
            break;
        }

        // copy_user_prompt for storing it in log
        char user_prompt_copy[PATH_MAX] = "";
        strcpy(user_prompt_copy, user_prompt);
        int flag_log = 0;

        // tokenize using ;
        char *token_semicolon;
        char *saveptr_semicolon;
        token_semicolon = strtok_r(user_prompt_copy, ";", &saveptr_semicolon);

        while (token_semicolon != NULL)
        {
            // tokenize using &
            int back_ground_no = 0;
            for (int j = 0; j < (int)strlen(token_semicolon); j++)
            {
                if (j != 0)
                {
                    if (token_semicolon[j] == '&')
                        back_ground_no++;
                }
            }
            char *token_ampersand;
            char *saveptr_ampersand;
            token_ampersand = strtok_r(token_semicolon, "&", &saveptr_ampersand);
            while (token_ampersand != NULL)
            {
                //  check what to tokenize
                int len = strlen(token_ampersand);
                int cnt_less_than = 0, cnt_greater_than = 0, cnt_2greater_than = 0, cnt_pipe = 0, pos_less = -1, pos_greater = -1, pos_2greater = -1;
                for (int i = 0; i < len; i++)
                {
                    if (token_ampersand[i] == '<')
                    {
                        cnt_less_than++;
                        if (pos_less == -1)
                        {
                            pos_less = i;
                        }
                    }
                    else if (((i + 1) < len) && (token_ampersand[i] == '>') && (token_ampersand[i + 1] == '>'))
                    {
                        cnt_2greater_than++;
                        if (pos_2greater == -1)
                        {
                            pos_2greater = i;
                        }
                        i++;
                    }
                    else if (token_ampersand[i] == '>')
                    {
                        cnt_greater_than++;
                        if (pos_greater == -1)
                        {
                            pos_greater = i;
                        }
                    }
                    else if (token_ampersand[i] == '|')
                        cnt_pipe++;
                }
                int sum_redirec = cnt_2greater_than + cnt_greater_than + cnt_less_than;

                // tokenize based on different operators
                if (cnt_pipe == 0 && sum_redirec > 0)
                {
                    if (!tokenise_and_execute_redirection(token_ampersand, cnt_less_than, cnt_greater_than, cnt_2greater_than, pos_less, pos_greater, pos_2greater, &back_ground_no, &flag_log, home_dir, prev_dir))
                    {
                        token_ampersand = strtok_r(NULL, "&", &saveptr_ampersand);
                        continue;
                    }
                }
                else if ((sum_redirec == 0) && (cnt_pipe > 0))
                {
                    // if (back_ground_no > 0)
                    // {
                    //     back_ground_no--;
                    //     printf(YELLOW);
                    //     printf("Not implemented pipes in bg\n");
                    //     printf(RESET);
                    // }
                    // else
                    // {
                    //     if (!tokenise_and_execute_pipe(token_ampersand, &flag_log, home_dir, prev_dir))
                    //     {
                    //         token_ampersand = strtok_r(NULL, "&", &saveptr_ampersand);
                    //         continue;
                    //     }
                    // }
                }
                else if ((sum_redirec > 0) && (cnt_pipe > 0))
                {
                }
                else if ((sum_redirec == 0) && (cnt_pipe == 0))
                {
                    char command[PATH_MAX] = "";
                    char arg[20][PATH_MAX] = {""};
                    int k = 0;
                    tokenise_space(token_ampersand, command, arg, &flag_log, &k);
                    if ((back_ground_no > 0))
                    {
                        back_ground_no--;
                        if (strcmp(command, "") != 0)
                        {
                            background(command, arg, k);
                        }
                    }
                    else
                    {
                        if (strcmp(command, "hop") == 0)
                        {
                            hop(arg, k, home_dir, prev_dir);
                        }
                        else if (strcmp(command, "reveal") == 0)
                        {
                            reveal(arg, k, home_dir, prev_dir);
                        }
                        else if (strcmp(command, "log") == 0)
                        {
                            if (k == 0)
                            {
                                FILE *file = fopen(history_path, "r");
                                if (!file)
                                {
                                    printf("\033[0;31mError:There is no history stored,This is your first command!\033[0m\n");
                                }
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
                                {
                                    printf("\033[0;31mError : Please provide index\033[0m\n");
                                }
                                else
                                {
                                    printf("\033[0;31mError : invalid argument\033[0m\n");
                                }
                            }
                            else if (k == 2)
                            {
                                if (is_all_digits(arg[1]))
                                {
                                    FILE *file = fopen(history_path, "r");
                                    if (!file)
                                    {
                                        printf("\033[0;31mError:There is no history stored,This is your first command!\033[0m\n");
                                    }
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
                                char pid_str[20] = "";
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
                }
                token_ampersand = strtok_r(NULL, "&", &saveptr_ampersand);
            }
            token_semicolon = strtok_r(NULL, ";", &saveptr_semicolon);
        }
        if (flag_log == 0)
        {
            log_history(user_prompt);
        }
    }
    return 0;
}