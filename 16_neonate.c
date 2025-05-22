#include "headers.h"

void set_nonblocking_input()
{
    struct termios newterminal;
    tcgetattr(STDIN_FILENO, &newterminal);          // Get current terminal attributes
    newterminal.c_lflag &= ~(ICANON | ECHO);        // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newterminal); // Apply the new attributes
}

void restore_input_mode()
{
    struct termios newterminal;
    tcgetattr(STDIN_FILENO, &newterminal);
    newterminal.c_lflag |= ICANON | ECHO; // Re-enable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newterminal);
}

int key_pressed()
{
    fd_set readfds;
    struct timeval tv = {0, 0}; // Non-blocking timeout

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

    if (result > 0)
    {
        char c;
        read(STDIN_FILENO, &c, 1);
        if (c == 'x')
        {
            return 1;
        }
    }
    return 0;
}

void fetch_recentprocess_pid()
{
    FILE *f;
    char buffer[256];
    int recent_pid;

    f = fopen("/proc/loadavg", "r");
    if (f == NULL)
    {
        fprintf(stderr, RED);
        perror("Error opening /proc/loadavg");
        fprintf(stderr, RESET);
        return;
    }

    if (fgets(buffer, sizeof(buffer), f) != NULL)
    {
        sscanf(buffer, "%*f %*f %*f %*s %d", &recent_pid);
        printf("%d\n", recent_pid);
    }
    else
    {
        fprintf(stderr, RED);
        perror("Error reading /proc/loadavg");
        fprintf(stderr, RESET);
    }

    fclose(f);
}

int is_all_digits(const char *str)
{
    if (*str == '\0')
    {
        return 0;
    }
    while (*str)
    {
        if (!isdigit(*str))
        {
            return 0;
        }
        str++;
    }
    return 1;
}

void neonate(char arg[20][4096], int k)
{
    if (k == 0)
    {
        printf(RED);
        printf("Error:Please provide a flag\n");
        printf(RESET);
    }
    else if (k == 1)
    {
        if (strcmp(arg[0], "-n") == 0)
        {
            printf(RED);
            printf("Please provide [time_arg]\n");
            printf(RESET);
        }
        else
        {
            printf(RED);
            printf("Error:Please provide a valid flag\n");
            printf(RESET);
        }
    }
    else if (k == 2)
    {
        if (strcmp(arg[0], "-n") == 0)
        {
            if (is_all_digits(arg[1]))
            {
                set_nonblocking_input(); // Enable non-blocking input

                int time_interval = atoi(arg[1]) * 1000;
                int it = time_interval;
                while (1)
                {
                    fetch_recentprocess_pid();
                    it = time_interval;
                    while (it > 0)
                    {
                        usleep(10000); 
                        it -= 10;     
                        if (key_pressed())
                        {
                            restore_input_mode(); // Restore terminal settings
                            return;
                        }
                    }
                    if (time_interval == 0)
                    {
                        if (key_pressed())
                        {
                            restore_input_mode();
                            return;
                        }
                    }
                }
                restore_input_mode();
                return;
            }
            else
            {
                printf(RED);
                printf("Error:Please provide a valid [time_arg]\n");
                printf(RESET);
            }
        }
        else
        {
            printf(RED);
            printf("Error:Please provide a valid flag\n");
            printf(RESET);
        }
    }
    else
    {
        printf(RED);
        printf("Error:Please provide a valid command\n");
        printf(RESET);
    }
}
