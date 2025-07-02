#include "headers.h"

#define MAX_ENTRIES 1024
void print_long_format(const char *path, const char *name)
{
    struct stat file_stat;
    char full_path[4096];
    snprintf(full_path, sizeof(full_path), "%s/%s", path, name);

    if (stat(full_path, &file_stat) == -1)
    {
        fprintf(stderr, "\033[0;31m");
        perror("stat");
        fprintf(stderr, "\033[0m");
        return;
    }

    // File type and permissions
    printf((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
    printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");

    printf(" %ld", file_stat.st_nlink);
    printf(" %s", getpwuid(file_stat.st_uid)->pw_name);
    printf(" %s", getgrgid(file_stat.st_gid)->gr_name);
    printf(" %ld", file_stat.st_size);

    // Time of last modification
    char time_buf[64];
    strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&file_stat.st_mtime));
    printf(" %s", time_buf);

    if (S_ISDIR(file_stat.st_mode))
    {
        printf("\033[0;34m %s\033[0m\n", name);
    }
    else
    {
        if ((file_stat.st_mode & S_IXUSR) || (file_stat.st_mode & S_IXGRP) || (file_stat.st_mode & S_IXOTH))
        {
            printf(GREEN);
            printf(" %s\n", name);
            printf(RESET);
        }
        else
        {
            printf(" %s\n", name);
        }
    }
}

int compare_dirent(const void *a, const void *b)
{
    struct dirent *entryA = *(struct dirent **)a;
    struct dirent *entryB = *(struct dirent **)b;
    return strcmp(entryA->d_name, entryB->d_name);
}

// Main Function to find all the files and directories in given directory
void reveal_crux(const char *path, int show_hidden, int show_long_format)
{
    DIR *dir;
    struct dirent *entry;
    struct dirent *entries[MAX_ENTRIES];
    int total_elements = 0;
    int count = 0;
    if ((dir = opendir(path)) == NULL)
    {
        fprintf(stderr, "\033[0;31m");
        perror("opendir");
        fprintf(stderr, "\033[0m");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (total_elements >= MAX_ENTRIES)
        {
            fprintf(stderr, "\033[0;31m");
            fprintf(stderr, "Too many entries\n");
            fprintf(stderr, "\033[0m");
            break;
        }
        if (!show_hidden && entry->d_name[0] == '.')
        {
            count++;
            continue;
        }
        struct dirent *entry_copy = malloc(sizeof(struct dirent));
        if (!entry_copy)
        {
            fprintf(stderr, "\033[0;31m");
            perror("malloc");
            fprintf(stderr, "\033[0m");
            exit(EXIT_FAILURE);
        }

        // Copy the entires read for sorting
        memcpy(entry_copy, entry, sizeof(struct dirent));
        entries[total_elements++] = entry_copy;
        count++;
    }
    closedir(dir);

    qsort(entries, total_elements, sizeof(struct dirent *), compare_dirent);
    if (show_hidden != 0 || show_long_format != 0)
        printf("total %d\n", count);


    for (int i = 0; i < total_elements; i++)
    {
        if (show_long_format)
        {
            print_long_format(path, entries[i]->d_name);
        }
        else
        {
            struct stat file_stat;
            char full_path[4096] = "";
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entries[i]->d_name);

            if (stat(full_path, &file_stat) == -1)
            {
                fprintf(stderr, "\033[0;31m");
                perror("stat");
                fprintf(stderr, "\033[0m");
                return;
            }
            if (S_ISDIR(file_stat.st_mode))
            {
                printf("\033[0;34m %s\033[0m\n", entries[i]->d_name);
            }
            else
            {
                if ((file_stat.st_mode & S_IXUSR) || (file_stat.st_mode & S_IXGRP) || (file_stat.st_mode & S_IXOTH))
                {
                    printf(GREEN);
                    printf(" %s\n", entries[i]->d_name);
                    printf(RESET);
                }
                else
                {
                    printf(" %s\n", entries[i]->d_name);
                }
            }
        }
    }
}

void reveal(char arg[20][PATH_MAX], int k, char *home_dir, char *prev_dir)
{
    char cwd[PATH_MAX];
    char path[PATH_MAX];
    int flag_a = 0;
    int flag_l = 0;
    if (k == 0)
    {
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            fprintf(stderr, "\033[0;31m");
            perror("getcwd() error");
            fprintf(stderr, "\033[0m");
            return;
        }
        strcpy(path, cwd);
        reveal_crux(path, 0, 0);
        return;
    }
    if (arg[k - 1][0] == '-')
    {
        if (strlen(arg[k - 1]) != 1)
        {
            for (int i = 0; i < k; i++)
            {
                int flag_length = strlen(arg[i]);
                int flag_check = 0;
                if (arg[i][0] == '-')
                {
                    for (int j = 1; j < flag_length; j++)
                    {
                        if (arg[i][j] != 'a' && arg[i][j] != 'l')
                        {
                            flag_check = 1;
                        }
                        else
                        {
                            if (flag_a != 1 || flag_l != 1)
                            {
                                if (arg[i][j] == 'a')
                                    flag_a = 1;
                                if (arg[i][j] == 'l')
                                    flag_l = 1;
                            }
                        }
                    }
                    if (flag_check == 1)
                    {
                        printf("\033[0;31m'%s' is not a valid flag\033[0m\n", arg[i]);
                        return;
                    }
                }
                else
                {
                    printf("\033[0;31mError:Multiple arguments are given for <path/name>\033[0m\n");
                    return;
                }
            }
            if (getcwd(cwd, sizeof(cwd)) == NULL)
            {
                fprintf(stderr, "\033[0;31m");
                perror("getcwd() error");
                fprintf(stderr, "\033[0m");
                return;
            }
            strcpy(path, cwd);
            reveal_crux(path, flag_a, flag_l);
        }
        else
        {
            for (int i = 0; i < k - 1; i++)
            {
                int flag_length = strlen(arg[i]);
                int flag_check = 0;
                if (arg[i][0] == '-')
                {
                    for (int j = 1; j < flag_length; j++)
                    {
                        if (arg[i][j] != 'a' && arg[i][j] != 'l')
                        {
                            flag_check = 1;
                        }
                        else
                        {
                            if (flag_a != 1 || flag_l != 1)
                            {
                                if (arg[i][j] == 'a')
                                    flag_a = 1;
                                if (arg[i][j] == 'l')
                                    flag_l = 1;
                            }
                        }
                    }
                    if (flag_check == 1)
                    {
                        printf("\033[0;31m'-%s' is not a valid flag\033[0m\n", arg[i]);
                        return;
                    }
                }
                else
                {
                    printf("\033[0;31mError:Multiple arguments are given for <path/name>\033[0m\n");
                    return;
                }
            }
            if (strcmp(prev_dir, "\0") == 0)
                printf("\033[0;31mERROR: previous_directory not set\033[0m\n");
            else
            {
                strcpy(path, prev_dir);
                reveal_crux(path, flag_a, flag_l);
            }
        }
    }
    else
    {
        for (int i = 0; i < k - 1; i++)
        {
            int flag_length = strlen(arg[i]);
            int flag_check = 0;
            if (arg[i][0] == '-')
            {
                for (int j = 1; j < flag_length; j++)
                {
                    if (arg[i][j] != 'a' && arg[i][j] != 'l')
                    {
                        flag_check = 1;
                    }
                    else
                    {
                        if (flag_a != 1 || flag_l != 1)
                        {
                            if (arg[i][j] == 'a')
                                flag_a = 1;
                            if (arg[i][j] == 'l')
                                flag_l = 1;
                        }
                    }
                }
                if (flag_check == 1)
                {
                    printf("\033[0;31m'-%s' is not a valid flag\033[0m\n", arg[i]);
                    return;
                }
            }
            else
            {
                printf("\033[0;31mError:Multiple arguments are given for <path/name>\033[0m\n");
                return;
            }
        }
        if (strcmp(arg[k - 1], "~") == 0)
        {
            strcpy(path, home_dir);
            reveal_crux(path, flag_a, flag_l);
        }
        else if (strcmp(arg[k - 1], ".") == 0)
        {
            if (getcwd(cwd, sizeof(cwd)) == NULL)
            {
                fprintf(stderr, "\033[0;31m");
                perror("getcwd() error");
                fprintf(stderr, "\033[0m");
                return;
            }
            strcpy(path, cwd);
            reveal_crux(path, flag_a, flag_l);
        }
        else if (strcmp(arg[k - 1], "..") == 0)
        {
            strcpy(path, arg[k - 1]);
            reveal_crux(path, flag_a, flag_l);
        }
        else
        {
            // for absolute paths
            if ((arg[k - 1][0] == '/'))
            {
                strcpy(path, arg[k - 1]);
                reveal_crux(path, flag_a, flag_l);
            }

            // for relative paths
            else if ((arg[k - 1][0] == '~') && (arg[k - 1][1] == '/'))
            {
                strcpy(path, home_dir);
                strcat(path, arg[k - 1] + 1);
                reveal_crux(path, flag_a, flag_l);
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
                strcat(path, arg[k - 1]);
                reveal_crux(path, flag_a, flag_l);
            }
        }
    }
}
