#include "headers.h"

extern time_t start;
extern char user_prompt[PATH_MAX];
extern int flag_first_prompt;

int display(char *home_dir, char *current_dir)
{
    // to find system name
    char temp[PATH_MAX];
    strcpy(temp, user_prompt);
    int temp_len = strlen(temp);
    if (temp[temp_len - 1] == '\n')
        temp[temp_len - 1] = '\0';
    char systemname[256];
    int result;
    result = gethostname(systemname, sizeof(systemname));
    if (result == -1)
    {
        fprintf(stderr, "\033[0;31m");
        perror("gethostname");
        fprintf(stderr, "\033[0m");
        return 1;
    }

    // to find user name
    struct passwd *pw;
    __uid_t uid;
    uid = getuid();
    pw = getpwuid(uid);
    if (pw == NULL)
    {
        fprintf(stderr, "\033[0;31m");
        perror("getpwuid");
        fprintf(stderr, "\033[0m");
        return 1;
    }

    // To display shell prompt including cwd
    double cpu_time_used;
    int rounded_time;
    time_t end;

    if (strcmp(home_dir, current_dir) == 0)
    {
        if (flag_first_prompt == 0)
        {
            time(&end);
            cpu_time_used = ((double)(end - start));
            rounded_time = (int)round(cpu_time_used);
            if (rounded_time > 2)
                printf("<\033[0;32m%s@%s:\033[0;34m~ \033[0m%s : %dsec>", pw->pw_name, systemname, temp, rounded_time);
            else
                printf("<\033[0;32m%s@%s:\033[0;34m~\033[0m>", pw->pw_name, systemname);
        }
        else
        {
            flag_first_prompt = 0;
            printf("<\033[0;32m%s@%s:\033[0;34m~\033[0m>", pw->pw_name, systemname);
        }
    }
    else if (strcmp(home_dir, current_dir) < 0)
    {
        int l1 = strlen(home_dir);
        int l2 = strlen(current_dir);
        char extra[l2 - l1 + 1];
        strcpy(extra, current_dir + l1);

        time(&end);
        cpu_time_used = ((double)(end - start));
        rounded_time = (int)round(cpu_time_used);

        if (rounded_time > 2)
            printf("<\033[0;32m%s@%s:\033[0;34m~%s \033[0m%s : %dsec>", pw->pw_name, systemname, extra, temp, rounded_time);
        else
            printf("<\033[0;32m%s@%s:\033[0;34m~%s\033[0m>", pw->pw_name, systemname, extra);
    }
    else
    {
        time(&end);
        cpu_time_used = ((double)(end - start));
        rounded_time = (int)round(cpu_time_used);

        if (rounded_time > 2)
            printf("<\033[0;32m%s@%s:\033[0;34m%s \033[0m%s : %dsec>", pw->pw_name, systemname, current_dir, temp, rounded_time);
        else
            printf("<\033[0;32m%s@%s:\033[0;34m%s\033[0m>", pw->pw_name, systemname, current_dir);
    }
}
