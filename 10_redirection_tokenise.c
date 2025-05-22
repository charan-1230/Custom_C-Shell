#include "headers.h"

void tokenise_space(char *input, char command[PATH_MAX], char arg[20][PATH_MAX], int *flag_log, int *k)
{
    char *token_space;
    char *saveptr_space;
    token_space = strtok_r(input, " \t", &saveptr_space);
    int flag = 0;
    while (token_space != NULL)
    {
        if ((flag) == 0)
        {
            strcpy(command, token_space);
            (flag) = 1;
            if (((*flag_log) == 0) && strcmp(command, "log") == 0)
            {
                (*flag_log) = 1;
            }
        }
        else
        {
            strcpy(arg[(*k)++], token_space);
        }
        token_space = strtok_r(NULL, " \t", &saveptr_space);
    }
}

int tokenise_and_execute_redirection(char *token_ampersand, int cnt_less_than, int cnt_greater_than, int cnt_2greater_than, int pos_less, int pos_greater, int pos_2greater, int *back_ground_no, int *flag_log, char *home_dir, char *prev_dir)
{
    if ((cnt_less_than == 1) && ((cnt_greater_than + cnt_2greater_than) == 0))
    {
        char command[PATH_MAX] = "";
        char arg[20][PATH_MAX] = {""};
        int k = 0;
        char input_file[PATH_MAX] = "";
        char *token_less;
        char *saveptr_less;
        if (token_ampersand[0] != '<')
        {
            token_less = strtok_r(token_ampersand, "<", &saveptr_less);
            if (token_less != NULL)
            {
                tokenise_space(token_less, command, arg, flag_log, &k);
            }
            token_less = strtok_r(NULL, "<", &saveptr_less);
        }
        else
        {
            token_ampersand++;
            token_less = strtok_r(token_ampersand, "<", &saveptr_less);
        }
        char *token_input_file;
        if (token_less != NULL)
        {
            token_input_file = strtok(token_less, " \t");
            if (token_input_file != NULL)
            {
                strcpy(input_file, token_input_file);
            }
            else
            {
                printf(RED);
                printf("Error:syntax error,missing value after '<'\n");
                printf(RESET);
            }
        }
        else
        {
            printf(RED);
            printf("Error:syntax error,missing value after '<'\n");
            printf(RESET);
        }
        if ((*back_ground_no) > 0)
        {
            (*back_ground_no)--;
            if (strcmp(input_file, "") != 0)
            {
                redirect_in_bg(command, arg, input_file, k, home_dir, prev_dir);
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if (strcmp(input_file, "") != 0)
            {
                redirect_in_fg(command, arg, input_file, k, home_dir, prev_dir);
            }
            else
            {
                return 0;
            }
        }
    }
    else if ((cnt_less_than == 0) && ((cnt_greater_than + cnt_2greater_than) == 1))
    {
        char output_file[PATH_MAX] = "";
        char *token_g;
        char *saveptr_g;
        char *token_output_file;
        if (cnt_greater_than == 1)
        {
            char command[PATH_MAX] = "";
            char arg[20][PATH_MAX] = {""};
            int k = 0;
            if (token_ampersand[0] != '>')
            {
                token_g = strtok_r(token_ampersand, ">", &saveptr_g);
                if (token_g != NULL)
                {
                    tokenise_space(token_g, command, arg, flag_log, &k);
                }
                token_g = strtok_r(NULL, ">", &saveptr_g);
            }
            else
            {
                token_ampersand++;
                token_g = strtok_r(token_ampersand, ">", &saveptr_g);
            }
            if (token_g != NULL)
            {
                token_output_file = strtok(token_g, " \t");
                if (token_output_file != NULL)
                {
                    strcpy(output_file, token_output_file);
                }
                else
                {
                    printf(RED);
                    printf("Error:syntax error,missing value after '>'\n");
                    printf(RESET);
                }
            }
            else
            {
                printf(RED);
                printf("Error:syntax error,missing value after '>'\n");
                printf(RESET);
            }
            if ((*back_ground_no) > 0)
            {
                (*back_ground_no)--;
                if (strcmp(output_file, "") != 0)
                {
                    redirect_out_bg(command, arg, output_file, k, home_dir, prev_dir);
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                if (strcmp(output_file, "") != 0)
                {
                    redirect_out_fg(command, arg, output_file, k, home_dir, prev_dir);
                }
                else
                {
                    return 0;
                }
            }
        }
        else if (cnt_2greater_than == 1)
        {
            char command[PATH_MAX] = "";
            char arg[20][PATH_MAX] = {""};
            int k = 0;
            if (token_ampersand[0] != '>')
            {
                token_g = strtok_r(token_ampersand, ">>", &saveptr_g);
                if (token_g != NULL)
                {
                    tokenise_space(token_g, command, arg, flag_log, &k);
                }
                token_g = strtok_r(NULL, ">>", &saveptr_g);
            }
            else
            {
                token_ampersand += 2;
                token_g = strtok_r(token_ampersand, ">>", &saveptr_g);
            }
            if (token_g != NULL)
            {
                token_output_file = strtok(token_g, " \t");
                if (token_output_file != NULL)
                {
                    strcpy(output_file, token_output_file);
                }
                else
                {
                    printf(RED);
                    printf("Error:syntax error,missing value after '>>'\n");
                    printf(RESET);
                }
            }
            else
            {
                printf(RED);
                printf("Error:syntax error,missing value after '>>'\n");
                printf(RESET);
            }
            if ((*back_ground_no) > 0)
            {
                (*back_ground_no)--;
                if (strcmp(output_file, "") != 0)
                {
                    redirect_out2_bg(command, arg, output_file, k, home_dir, prev_dir);
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                if (strcmp(output_file, "") != 0)
                {
                    redirect_out2_fg(command, arg, output_file, k, home_dir, prev_dir);
                }
                else
                {
                    return 0;
                }
            }
        }
    }
    else if ((cnt_less_than == 1) && ((cnt_greater_than + cnt_2greater_than) == 1))
    {
        char input_file[PATH_MAX] = "";
        char *token_less;
        char *saveptr_less;
        char *token_input_file;
        char output_file[PATH_MAX] = "";
        char *token_g;
        char *saveptr_g;
        char *token_output_file;
        int flag_both = 0;
        if (cnt_greater_than == 1)
        {
            char command[PATH_MAX] = "";
            char arg[20][PATH_MAX] = {""};
            int k = 0;
            if (pos_less < pos_greater)
            {
                if (token_ampersand[0] != '<')
                {
                    token_less = strtok_r(token_ampersand, "<", &saveptr_less);
                    if (token_less != NULL)
                    {
                        tokenise_space(token_less, command, arg, flag_log, &k);
                    }
                    token_less = strtok_r(NULL, "<", &saveptr_less);
                }
                else
                {
                    token_ampersand++;
                    token_less = strtok_r(token_ampersand, "<", &saveptr_less);
                }
                if (token_less != NULL)
                {

                    token_g = strtok_r(token_less, ">", &saveptr_g);
                    if (token_g != NULL)
                    {
                        token_input_file = strtok(token_g, " \t");
                        if (token_input_file != NULL)
                        {
                            strcpy(input_file, token_input_file);
                        }
                        else
                        {
                            printf(RED);
                            printf("Error:syntax error,missing value after '<'\n");
                            printf(RESET);
                        }
                    }
                    else
                    {
                        printf(RED);
                        printf("Error:syntax error,missing value after '<'\n");
                        printf(RESET);
                    }
                    token_g = strtok_r(NULL, ">", &saveptr_g);
                    if (token_g != NULL)
                    {
                        token_output_file = strtok(token_g, " \t");
                        if (token_output_file != NULL)
                        {
                            strcpy(output_file, token_output_file);
                        }
                        else
                        {
                            printf(RED);
                            printf("Error:syntax error,missing value after '>'\n");

                            printf(RESET);
                        }
                    }
                    else
                    {
                        printf(RED);
                        printf("Error:syntax error\n");
                        strcpy(input_file, "");
                        printf(RESET);
                    }
                }
            }
            else
            {
                if (token_ampersand[0] != '>')
                {
                    token_g = strtok_r(token_ampersand, ">", &saveptr_g);
                    if (token_g != NULL)
                    {
                        tokenise_space(token_g, command, arg, flag_log, &k);
                    }
                    token_g = strtok_r(NULL, ">", &saveptr_g);
                }
                else
                {
                    token_ampersand++;
                    token_g = strtok_r(token_ampersand, ">", &saveptr_g);
                }
                if (token_g != NULL)
                {
                    token_less = strtok_r(token_g, "<", &saveptr_less);
                    if (token_less != NULL)
                    {
                        token_output_file = strtok(token_less, " \t");
                        if (token_output_file != NULL)
                        {
                            strcpy(output_file, token_output_file);
                        }
                        else
                        {
                            printf(RED);
                            printf("Error:syntax error,missing value after '>'\n");
                            printf(RESET);
                        }
                    }
                    else
                    {
                        printf(RED);
                        printf("Error:syntax error,missing value after '>'\n");
                        printf(RESET);
                    }
                    token_less = strtok_r(NULL, "<", &saveptr_less);
                    if (token_less != NULL)
                    {
                        token_input_file = strtok(token_less, " \t");
                        if (token_input_file != NULL)
                        {
                            strcpy(input_file, token_input_file);
                        }
                        else
                        {
                            printf(RED);
                            printf("Error:syntax error,missing value after '<'\n");
                            printf(RESET);
                        }
                    }
                    else
                    {
                        printf(RED);
                        printf("Error:syntax error\n");
                        strcpy(output_file, "");
                        printf(RESET);
                    }
                }
            }
            if ((*back_ground_no) > 0)
            {
                (*back_ground_no)--;
                if ((strcmp(output_file, "") != 0) && (strcmp(input_file, "") != 0))
                {
                    redirect_inout_bg(command, arg, input_file, output_file, k, home_dir, prev_dir);
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                if ((strcmp(output_file, "") != 0) && (strcmp(input_file, "") != 0))
                {
                    redirect_inout_fg(command, arg, input_file, output_file, k, home_dir, prev_dir);
                }
                else
                {
                    return 0;
                }
            }
        }
        else if (cnt_2greater_than == 1)
        {
            char command[PATH_MAX] = "";
            char arg[20][PATH_MAX] = {""};
            int k = 0;
            if (pos_less < pos_2greater)
            {
                if (token_ampersand[0] != '<')
                {
                    token_less = strtok_r(token_ampersand, "<", &saveptr_less);
                    if (token_less != NULL)
                    {
                        tokenise_space(token_less, command, arg, flag_log, &k);
                    }
                    token_less = strtok_r(NULL, "<", &saveptr_less);
                }
                else
                {
                    token_ampersand++;
                    token_less = strtok_r(token_ampersand, "<", &saveptr_less);
                }
                if (token_less != NULL)
                {
                    token_g = strtok_r(token_less, ">>", &saveptr_g);
                    if (token_g != NULL)
                    {
                        token_input_file = strtok(token_g, " \t");
                        if (token_input_file != NULL)
                        {
                            strcpy(input_file, token_input_file);
                        }
                        else
                        {
                            printf(RED);
                            printf("Error:syntax error,missing value after '<'\n");
                            printf(RESET);
                        }
                    }
                    else
                    {
                        printf(RED);
                        printf("Error:syntax error,missing value after '<'\n");
                        printf(RESET);
                    }
                    token_g = strtok_r(NULL, ">>", &saveptr_g);
                    if (token_g != NULL)
                    {
                        token_output_file = strtok(token_g, " \t");
                        if (token_output_file != NULL)
                        {
                            strcpy(output_file, token_output_file);
                        }
                        else
                        {
                            printf(RED);
                            printf("Error:syntax error,missing value after '>>'\n");
                            printf(RESET);
                        }
                    }
                    else
                    {
                        printf(RED);
                        printf("Error:syntax error\n");
                        printf(RESET);
                    }
                }
            }
            else
            {

                if (token_ampersand[0] != '>')
                {
                    token_g = strtok_r(token_ampersand, ">>", &saveptr_g);
                    if (token_g != NULL)
                    {
                        tokenise_space(token_g, command, arg, flag_log, &k);
                    }
                    token_g = strtok_r(NULL, ">>", &saveptr_g);
                }
                else
                {
                    token_ampersand += 2;
                    token_g = strtok_r(token_ampersand, ">>", &saveptr_g);
                }
                if (token_g != NULL)
                {
                    token_less = strtok_r(token_g, "<", &saveptr_less);
                    if (token_less != NULL)
                    {
                        token_output_file = strtok(token_less, " \t");
                        if (token_output_file != NULL)
                        {
                            strcpy(output_file, token_output_file);
                        }
                        else
                        {
                            printf(RED);
                            printf("Error:syntax error,missing value after '>>'\n");
                            printf(RESET);
                        }
                    }
                    else
                    {
                        printf(RED);
                        printf("Error:syntax error,missing value after '>>'\n");
                        printf(RESET);
                    }
                    token_less = strtok_r(NULL, "<", &saveptr_less);
                    if (token_less != NULL)
                    {
                        token_input_file = strtok(token_less, " \t");
                        if (token_input_file != NULL)
                        {
                            strcpy(input_file, token_input_file);
                        }
                        else
                        {
                            printf(RED);
                            printf("Error:syntax error,missing value after '<'\n");
                            printf(RESET);
                        }
                    }
                    else
                    {
                        printf(RED);
                        printf("Error:syntax error\n");
                        printf(RESET);
                    }
                }
            }
            if ((*back_ground_no) > 0)
            {
                (*back_ground_no)--;
                if ((strcmp(output_file, "") != 0) && (strcmp(input_file, "") != 0))
                {
                    redirect_inout2_bg(command, arg, input_file, output_file, k, home_dir, prev_dir);
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                if ((strcmp(output_file, "") != 0) && (strcmp(input_file, "") != 0))
                {
                    redirect_inout2_fg(command, arg, input_file, output_file, k, home_dir, prev_dir);
                }
                else
                {
                    return 0;
                }
            }
        }
    }
}