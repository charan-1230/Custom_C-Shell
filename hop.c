#include "headers.h"

void change_directory(const char *path)
{
    if (chdir(path) != 0)
    {
        fprintf(stderr, "\033[0;31m");
        perror("chdir() error");
        fprintf(stderr, "\033[0m");
    }
    else
    {
        char cwd[PATH_MAX];
        if (getcwd(cwd, PATH_MAX) != NULL)
            printf("%s\n", cwd);
        else
        {
            fprintf(stderr, "\033[0;31m");
            perror("getcwd() error");
            fprintf(stderr, "\033[0m");
        }
    }
}

void hop(char arg[20][PATH_MAX], int k, char *home_dir, char *prev_dir)
{
    char cwd[PATH_MAX];
    if (k == 0)
    {
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            fprintf(stderr, "\033[0;31m");
            perror("getcwd() error");
            fprintf(stderr, "\033[0m");
            return;
        }
        strcpy(prev_dir, cwd);
        change_directory(home_dir);
    }
    else
    {
        char path[PATH_MAX];
        for (int i = 0; i < k; i++)
        {
            if (strcmp(arg[i], "~") == 0)
            {
                if (getcwd(cwd, sizeof(cwd)) == NULL)
                {
                    fprintf(stderr, "\033[0;31m");
                    perror("getcwd() error");
                    fprintf(stderr, "\033[0m");
                    return;
                }
                change_directory(home_dir);
                strcpy(prev_dir, cwd);
            }
            else if (strcmp(arg[i], ".") == 0)
            {
                if (getcwd(cwd, sizeof(cwd)) == NULL)
                {
                    fprintf(stderr, "\033[0;31m");
                    perror("getcwd() error");
                    fprintf(stderr, "\033[0m");
                    return;
                }
                strcpy(prev_dir, cwd);
                change_directory(cwd);
            }
            else if (strcmp(arg[i], "..") == 0)
            {
                if (getcwd(cwd, sizeof(cwd)) == NULL)
                {
                    fprintf(stderr, "\033[0;31m");
                    perror("getcwd() error");
                    fprintf(stderr, "\033[0m");
                    return;
                }
                strcpy(prev_dir, cwd);
                strcpy(path, arg[i]);
                change_directory(path);
            }
            else if (strcmp(arg[i], "-") == 0)
            {
                if (strcmp(prev_dir, "\0") == 0)
                    printf("\033[0;31mERROR: previous_directory not set\033[0m\n");
                else
                {
                    if (getcwd(cwd, sizeof(cwd)) == NULL)
                    {
                        fprintf(stderr, "\033[0;31m");
                        perror("getcwd() error");
                        fprintf(stderr, "\033[0m");
                        return;
                    }
                    strcpy(path, prev_dir);
                    change_directory(path);
                    strcpy(prev_dir, cwd);
                }
            }
            else
            {
                // for absolute paths
                if ((arg[i][0] == '/'))
                {
                    if (getcwd(cwd, sizeof(cwd)) == NULL)
                    {
                        fprintf(stderr, "\033[0;31m");
                        perror("getcwd() error");
                        fprintf(stderr, "\033[0m");
                        return;
                    }
                    strcpy(prev_dir, cwd);
                    change_directory(arg[i]);
                }

                // for relative paths
                else if ((arg[i][0] == '~') && (arg[i][1] == '/'))
                {
                    if (getcwd(cwd, sizeof(cwd)) == NULL)
                    {
                        fprintf(stderr, "\033[0;31m");
                        perror("getcwd() error");
                        fprintf(stderr, "\033[0m");
                        return;
                    }
                    strcpy(prev_dir, cwd);
                    strcpy(path, home_dir);
                    strcat(path, arg[i] + 1);
                    change_directory(path);
                }

                // for paths from home dir
                else
                {
                    if (getcwd(cwd, sizeof(cwd)) == NULL)
                    {
                        fprintf(stderr, "\033[0;31m");
                        perror("getcwd() error");
                        fprintf(stderr, "\033[0m");
                        return;
                    }
                    strcpy(path, cwd);
                    strcat(path, "/");
                    strcat(path, arg[i]);
                    change_directory(path);
                    strcpy(prev_dir, cwd);
                }
            }
        }
    }
}
