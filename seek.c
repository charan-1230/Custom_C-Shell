#include "headers.h"

void print_error(const char *message)
{
    fprintf(stderr, RED);
    fprintf(stderr, "%s\n", message);
    fprintf(stderr, RESET);
}

int is_directory(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

int is_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

// Function to check if 'target' is a prefix of 'name'
int is_prefix(const char *name, const char *target)
{
    return strncmp(name, target, strlen(target)) == 0;
}

void print_relative_path(const char *full_path, const char *base_dir)
{
    if (strncmp(full_path, base_dir, strlen(base_dir)) == 0)
    {
        printf("./%s\n", full_path + strlen(base_dir) + 1);
    }
    else
    {
        printf("%s\n", full_path);
    }
}

// for -e flag
void print_file_content(const char *file_path)
{
    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {
        fprintf(stderr, RED);
        perror("fopen");
        fprintf(stderr, RESET);
        return;
    }

    char ch;
    while ((ch = fgetc(file)) != EOF)
    {
        putchar(ch);
    }
    fclose(file);
}

void search_directory(const char *dir_path, const char *target, const char *base_dir, int flag_d, int flag_f, int flag_e, int *match_count, char *found_path)
{
    struct dirent *entry;
    DIR *dir = opendir(dir_path);
    if (!dir)
    {
        fprintf(stderr, RED);
        perror("opendir");
        fprintf(stderr, RESET);
        return;
    }

    char path[PATH_MAX];
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        if (is_directory(path))
        {
            if (!flag_f && is_prefix(entry->d_name, target))
            {
                printf(BLUE);
                print_relative_path(path, base_dir);
                printf(RESET);
                (*match_count)++;
                strcpy(found_path, path);
            }
            search_directory(path, target, base_dir, flag_d, flag_f, flag_e, match_count, found_path);
        }
        else if (is_file(path))
        {
            if (!flag_d && is_prefix(entry->d_name, target))
            {
                printf(GREEN);
                print_relative_path(path, base_dir);
                printf(RESET);
                (*match_count)++;
                strcpy(found_path, path);
            }
        }
    }
    closedir(dir);
}

void seek_crux(int flag_d, int flag_e, int flag_f, char *search, char *target_directory)
{
    if (flag_d && flag_f)
    {
        print_error("Invalid flags!");
        return;
    }

    int match_count = 0;
    char found_path[PATH_MAX] = "";
    search_directory(target_directory, search, target_directory, flag_d, flag_f, flag_e, &match_count, found_path);

    if (match_count == 0)
    {
        printf("No match found!\n");
        return;
    }

    // handling single match case for -e flag
    if (flag_e && match_count == 1)
    {
        if (is_directory(found_path))
        {
            if (access(found_path, X_OK) == 0)
            {
                chdir(found_path);
            }
            else
            {
                printf("Missing permissions for task!\n");
            }
        }
        else if (is_file(found_path))
        {
            if (access(found_path, R_OK) == 0)
            {
                print_file_content(found_path);
            }
            else
            {
                printf("Missing permissions for task!\n");
            }
        }
    }
}

void set_path(char *path, char *input_path, char *home_dir, char *prev_dir)
{
    char cwd[PATH_MAX];
    if (strcmp(input_path, "~") == 0)
    {
        strcpy(path, home_dir);
    }
    else if (strcmp(input_path, ".") == 0)
    {
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            fprintf(stderr, "\033[0;31m");
            perror("getcwd() error");
            fprintf(stderr, "\033[0m");
            return;
        }
        strcpy(path, cwd);
    }
    else if (strcmp(input_path, "..") == 0)
    {
        strcpy(path, input_path);
    }
    else if (strcmp(input_path, "-") == 0)
    {
        if (strcmp(prev_dir, "\0") == 0)
            printf("\033[0;31mERROR: previous_directory not set\033[0m\n");
        else
        {
            strcpy(path, prev_dir);
        }
    }
    else
    {
        // for absolute paths
        if ((input_path[0] == '/'))
        {
            strcpy(path, input_path);
        }

        // for relative paths
        else if ((input_path[0] == '~') && (input_path[1] == '/'))
        {
            strcpy(path, home_dir);
            strcat(path, input_path + 1);
        }

        // for paths from curr dir
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
            strcat(path, input_path);
        }
    }
}

void seek(char arg[20][PATH_MAX], int k, char *home_dir, char *prev_dir)
{
    char cwd[PATH_MAX];
    char path[PATH_MAX];
    int flag_d = 0;
    int flag_e = 0;
    int flag_f = 0;
    int executed = 0;
    if (k == 0)
    {
        printf(RED);
        printf("Error:please provide a search name\n");
        printf(RESET);
        return;
    }
    if (k == 1)
    {
        if ((strcmp(arg[0], ".") == 0) || (strcmp(arg[0], "..") == 0) || strchr(arg[0], '/') != NULL)
        {
            printf(RED);
            printf("Error:please provide a valid search name\n");
            printf(RESET);
            return;
        }
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            fprintf(stderr, "\033[0;31m");
            perror("getcwd() error");
            fprintf(stderr, "\033[0m");
            return;
        }
        strcpy(path, cwd);
        seek_crux(flag_d, flag_e, flag_f, arg[0], path);
        return;
    }
    for (int i = 0; i < k; i++)
    {
        if (arg[i][0] == '-')
        {
            if (strlen(arg[i]) != 2)
            {
                if ((k - i) > 2)
                {
                    printf(RED);
                    printf("Error:Invalid command\n");
                    printf(RESET);
                    return;
                }
                else if ((k - i) == 2)
                {
                    if (strlen(arg[i]) == 1)
                    {
                        set_path(path, arg[k - 1], home_dir, prev_dir);
                        seek_crux(flag_d, flag_e, flag_f, arg[i], path);
                        return;
                    }
                    else
                    {
                        if (strchr(arg[i], '/') != NULL)
                        {
                            printf(RED);
                            printf("Error:please provide a valid search name\n");
                            printf(RESET);
                            return;
                        }
                        else
                        {
                            set_path(path, arg[k - 1], home_dir, prev_dir);
                            seek_crux(flag_d, flag_e, flag_f, arg[i], path);
                            return;
                        }
                    }
                }
                else
                {
                    if (getcwd(cwd, sizeof(cwd)) == NULL)
                    {
                        fprintf(stderr, "\033[0;31m");
                        perror("getcwd() error");
                        fprintf(stderr, "\033[0m");
                        return;
                    }
                    if (strlen(arg[i]) == 1)
                    {
                        seek_crux(flag_d, flag_e, flag_f, arg[i], cwd);
                        return;
                    }
                    else
                    {
                        if (strchr(arg[i], '/') != NULL)
                        {
                            printf(RED);
                            printf("Error:please provide a valid search name\n");
                            printf(RESET);
                            return;
                        }
                        seek_crux(flag_d, flag_e, flag_f, arg[i], cwd);
                        return;
                    }
                }
            }
            else
            {
                if (arg[i][1] == 'd')
                {
                    if ((k - i) >= 2)
                    {
                        flag_d = 1;
                    }
                    else
                    {
                        if (getcwd(cwd, sizeof(cwd)) == NULL)
                        {
                            fprintf(stderr, "\033[0;31m");
                            perror("getcwd() error");
                            fprintf(stderr, "\033[0m");
                            return;
                        }
                        seek_crux(flag_d, flag_e, flag_f, arg[i], cwd);
                        return;
                    }
                }
                else if (arg[i][1] == 'e')
                {
                    if ((k - i) >= 2)
                    {
                        flag_e = 1;
                    }
                    else
                    {
                        if (getcwd(cwd, sizeof(cwd)) == NULL)
                        {
                            fprintf(stderr, "\033[0;31m");
                            perror("getcwd() error");
                            fprintf(stderr, "\033[0m");
                            return;
                        }
                        seek_crux(flag_d, flag_e, flag_f, arg[i], cwd);
                        return;
                    }
                }
                else if (arg[i][1] == 'f')
                {
                    if ((k - i) >= 2)
                    {
                        flag_f = 1;
                    }
                    else
                    {
                        if (getcwd(cwd, sizeof(cwd)) == NULL)
                        {
                            fprintf(stderr, "\033[0;31m");
                            perror("getcwd() error");
                            fprintf(stderr, "\033[0m");
                            return;
                        }
                        seek_crux(flag_d, flag_e, flag_f, arg[i], cwd);
                        return;
                    }
                }
                else if ((arg[i][1] != 'd') && (arg[i][1] != 'e') && (arg[i][1] != 'f'))
                {
                    if ((k - i) > 2)
                    {
                        printf(RED);
                        printf("Error:Invalid command\n");
                        printf(RESET);
                        return;
                    }
                    else if ((k - i) == 2)
                    {
                        if (strchr(arg[i], '/') != NULL)
                        {
                            printf(RED);
                            printf("Error:please provide a valid search name\n");
                            printf(RESET);
                            return;
                        }
                        else
                        {
                            set_path(path, arg[k - 1], home_dir, prev_dir);
                            seek_crux(flag_d, flag_e, flag_f, arg[i], path);
                            return;
                        }
                    }
                    else
                    {
                        if (strchr(arg[i], '/') != NULL)
                        {
                            printf(RED);
                            printf("Error:please provide a valid search name\n");
                            printf(RESET);
                            return;
                        }
                        else
                        {
                            if (getcwd(cwd, sizeof(cwd)) == NULL)
                            {
                                fprintf(stderr, "\033[0;31m");
                                perror("getcwd() error");
                                fprintf(stderr, "\033[0m");
                                return;
                            }
                            seek_crux(flag_d, flag_e, flag_f, arg[i], cwd);
                            return;
                        }
                    }
                }
            }
        }
        else
        {
            if ((k - i) > 2)
            {
                printf(RED);
                printf("Error:Invalid command\n");
                printf(RESET);
                return;
            }
            else if ((k - i) == 2)
            {
                if ((strcmp(arg[i], ".") == 0) || (strcmp(arg[i], "..") == 0) || strchr(arg[i], '/') != NULL)
                {
                    printf(RED);
                    printf("Error:please provide a valid search name\n");
                    printf(RESET);
                    return;
                }
                else
                {
                    set_path(path, arg[k - 1], home_dir, prev_dir);
                    seek_crux(flag_d, flag_e, flag_f, arg[i], path);
                    return;
                }
            }
            else
            {
                if ((strcmp(arg[i], ".") == 0) || (strcmp(arg[i], "..") == 0) || (strchr(arg[i], '/') != NULL))
                {
                    printf(RED);
                    printf("Error:please provide a valid search name\n");
                    printf(RESET);
                    return;
                }
                else
                {
                    if (getcwd(cwd, sizeof(cwd)) == NULL)
                    {
                        fprintf(stderr, "\033[0;31m");
                        perror("getcwd() error");
                        fprintf(stderr, "\033[0m");
                        return;
                    }
                    seek_crux(flag_d, flag_e, flag_f, arg[i], cwd);
                    return;
                }
            }
        }
    }
}
