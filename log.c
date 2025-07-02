#include "headers.h"

extern char history_path[100];

void log_history(char *user_prompt)
{
    strcat(user_prompt, "\n");
    FILE *file = fopen(history_path, "r");
    if (!file)
    {
        file = fopen(history_path, "w");
        if (!file)
        {
            fprintf(stderr, "\033[0;31m");
            perror("Error opening file in write mode");
            fprintf(stderr, "\033[0m");
            return;
        }
        fputs(user_prompt, file);
        fclose(file);
    }
    else
    {
        char temp_store[15][PATH_MAX];
        int log_history_count = 0;

        while (fgets(temp_store[log_history_count], PATH_MAX, file) && log_history_count < 15)
        {
            log_history_count++;
        }
        fclose(file);

        // Shift 14 most recent lines if there are 15 lines already
        if (strcmp(temp_store[log_history_count - 1], user_prompt) != 0)
        {
            if (log_history_count == 15)
            {
                for (int i = 1; i < 15; i++)
                {
                    strcpy(temp_store[i - 1], temp_store[i]);
                }
                log_history_count--;
            }

            // Add the new line
            strcpy(temp_store[log_history_count], user_prompt);
            log_history_count++;

            file = fopen(history_path, "w");
            for (int i = 0; i < log_history_count; i++)
            {
                fputs(temp_store[i], file);
            }
            fclose(file);
        }
    }
}

void log_execute(char *input, char *home_dir, char *prev_dir)
{
    // tokenize using ;
    char *token_semicolon;
    char *saveptr_semicolon;
    token_semicolon = strtok_r(input, ";", &saveptr_semicolon);
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
            // tokenize using space
            char *token_space;
            char *saveptr_space;
            token_space = strtok_r(token_ampersand, " \t", &saveptr_space);
            int flag = 0;
            char command[PATH_MAX] = "";
            char arg[20][PATH_MAX] = {""};
            int k = 0;
            while (token_space != NULL)
            {
                if (flag == 0)
                {
                    strcpy(command, token_space);
                    flag = 1;
                }
                else
                {
                    strcpy(arg[k++], token_space);
                }
                token_space = strtok_r(NULL, " \t", &saveptr_space);
            }
            if ((back_ground_no > 0))
            {
                //  run all commands except last in background
                back_ground_no--;
                if (strcmp(command, "") != 0)
                {
                    background(command, arg, k);
                    printf("b->%d\n", back_ground_no);
                }
            }
            else
            {
                // run last command in foreground
                if (strcmp(command, "hop") == 0)
                {
                    hop(arg, k, home_dir, prev_dir);
                }
                else if (strcmp(command, "reveal") == 0)
                {
                    reveal(arg, k, home_dir, prev_dir);
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
            token_ampersand = strtok_r(NULL, "&", &saveptr_ampersand); // Continue tokenizing the string
        }
        token_semicolon = strtok_r(NULL, ";", &saveptr_semicolon); // Continue tokenizing the string
    }
}
