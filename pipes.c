#include "headers.h"

int tokenise_and_execute_pipe(char *token_ampersand, int *flag_log, char *home_dir, char *prev_dir)
{
    // Check for invalid pipe usage
    if (token_ampersand[0] == '|' || token_ampersand[strlen(token_ampersand) - 1] == '|')
    {
        printf(RED);
        printf("Error: Invalid use of pipe\n");
        printf(RESET);
        return 0;
    }
    
    // Check for consecutive pipes
    for (int i = 0; i < strlen(token_ampersand) - 1; i++)
    {
        if (token_ampersand[i] == '|' && token_ampersand[i + 1] == '|')
        {
            printf(RED);
            printf("Error: Invalid use of pipe\n");
            printf(RESET);
            return 0;
        }
    }
    
    // Split commands by pipe
    char commands[20][PATH_MAX];
    int num_commands = 0;
    char *token_pipe;
    char *saveptr_pipe;
    
    token_pipe = strtok_r(token_ampersand, "|", &saveptr_pipe);
    while (token_pipe != NULL && num_commands < 20)
    {
        // Trim whitespace
        while (*token_pipe == ' ' || *token_pipe == '\t') token_pipe++;
        char *end = token_pipe + strlen(token_pipe) - 1;
        while (end > token_pipe && (*end == ' ' || *end == '\t')) end--;
        *(end + 1) = '\0';
        
        if (strlen(token_pipe) == 0)
        {
            printf(RED);
            printf("Error: Invalid use of pipe\n");
            printf(RESET);
            return 0;
        }
        
        strcpy(commands[num_commands], token_pipe);
        num_commands++;
        token_pipe = strtok_r(NULL, "|", &saveptr_pipe);
    }
    
    if (num_commands < 2)
    {
        printf(RED);
        printf("Error: Invalid use of pipe\n");
        printf(RESET);
        return 0;
    }
    
    execute_pipe_commands(commands, num_commands, home_dir, prev_dir);
    return 1;
}

void execute_pipe_commands(char commands[][PATH_MAX], int num_commands, char *home_dir, char *prev_dir)
{
    int pipes[20][2]; // Support up to 20 pipes
    pid_t pids[20];
    
    // Create all pipes
    for (int i = 0; i < num_commands - 1; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            printf(RED);
            printf("Error: Failed to create pipe\n");
            printf(RESET);
            return;
        }
    }
    
    // Execute each command
    for (int i = 0; i < num_commands; i++)
    {
        char final_command[PATH_MAX];
        char input_file[PATH_MAX] = "";
        char output_file[PATH_MAX] = "";
        int append_mode = 0;
        
        // Parse command for redirection
        if (!parse_pipe_with_redirection(commands[i], final_command, input_file, output_file, &append_mode))
        {
            for (int j = 0; j < num_commands - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            return;
        }
        
        pids[i] = fork();
        if (pids[i] == -1)
        {
            printf(RED);
            printf("Error: Failed to fork process\n");
            printf(RESET);
            return;
        }
        
        if (pids[i] == 0) 
        {
            // Handle input redirection or pipe input
            if (i == 0 && strlen(input_file) > 0)
            {
                // First command with input redirection
                int input_fd = open(input_file, O_RDONLY);
                if (input_fd == -1)
                {
                    printf(RED);
                    printf("Error: No such input file found!\n");
                    printf(RESET);
                    exit(1);
                }
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }
            else if (i > 0)
            {
                // Not first command, get input from previous pipe
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            
            // Handle output redirection or pipe output
            if (i == num_commands - 1 && strlen(output_file) > 0)
            {
                // Last command with output redirection
                int output_fd;
                if (append_mode)
                    output_fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                else
                    output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                
                if (output_fd == -1)
                {
                    printf(RED);
                    printf("Error: Unable to open output file\n");
                    printf(RESET);
                    exit(1);
                }
                dup2(output_fd, STDOUT_FILENO);
                close(output_fd);
            }
            else if (i < num_commands - 1)
            {
                // Not last command, send output to next pipe
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            // Close all pipe descriptors in child
            for (int j = 0; j < num_commands - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            execute_single_pipe_command(final_command, home_dir, prev_dir);
            exit(0);
        }
    }
    
    // Close all pipes in parent
    for (int i = 0; i < num_commands - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    for (int i = 0; i < num_commands; i++)
    {
        int status;
        waitpid(pids[i], &status, 0);
    }
}

int parse_pipe_with_redirection(char *command_str, char *final_command, char *input_file, char *output_file, int *append_mode)
{
    strcpy(final_command, command_str);
    strcpy(input_file, "");
    strcpy(output_file, "");
    *append_mode = 0;
    
    // Check for output redirection first (>> then >)
    char *redirect_pos = strstr(command_str, ">>");
    if (redirect_pos != NULL)
    {
        *append_mode = 1;
        *redirect_pos = '\0';
        strcpy(final_command, command_str);
        
        // Trim whitespace from command
        char *end = final_command + strlen(final_command) - 1;
        while (end > final_command && (*end == ' ' || *end == '\t')) end--;
        *(end + 1) = '\0';
        
        // Get output file
        char *output_start = redirect_pos + 2;
        while (*output_start == ' ' || *output_start == '\t') output_start++;
        
        // Check for input redirection after >>
        char *input_redirect = strchr(output_start, '<');
        if (input_redirect != NULL)
        {
            *input_redirect = '\0';
            char *input_start = input_redirect + 1;
            while (*input_start == ' ' || *input_start == '\t') input_start++;
            
            char *input_end = input_start;
            while (*input_end != '\0' && *input_end != ' ' && *input_end != '\t') input_end++;
            *input_end = '\0';
            
            if (strlen(input_start) == 0)
            {
                printf(RED);
                printf("Error: syntax error, missing value after '<'\n");
                printf(RESET);
                return 0;
            }
            strcpy(input_file, input_start);
        }
        
        // Get output filename
        char *output_end = output_start;
        while (*output_end != '\0' && *output_end != ' ' && *output_end != '\t' && *output_end != '<') output_end++;
        *output_end = '\0';
        
        if (strlen(output_start) == 0)
        {
            printf(RED);
            printf("Error: syntax error, missing value after '>>'\n");
            printf(RESET);
            return 0;
        }
        strcpy(output_file, output_start);
    }
    else
    {
        redirect_pos = strchr(command_str, '>');
        if (redirect_pos != NULL)
        {
            *redirect_pos = '\0';
            strcpy(final_command, command_str);
            
            // Trim whitespace from command
            char *end = final_command + strlen(final_command) - 1;
            while (end > final_command && (*end == ' ' || *end == '\t')) end--;
            *(end + 1) = '\0';
            
            // Get output file
            char *output_start = redirect_pos + 1;
            while (*output_start == ' ' || *output_start == '\t') output_start++;
            
            // Check for input redirection after >
            char *input_redirect = strchr(output_start, '<');
            if (input_redirect != NULL)
            {
                *input_redirect = '\0';
                char *input_start = input_redirect + 1;
                while (*input_start == ' ' || *input_start == '\t') input_start++;
                
                char *input_end = input_start;
                while (*input_end != '\0' && *input_end != ' ' && *input_end != '\t') input_end++;
                *input_end = '\0';
                
                if (strlen(input_start) == 0)
                {
                    printf(RED);
                    printf("Error: syntax error, missing value after '<'\n");
                    printf(RESET);
                    return 0;
                }
                strcpy(input_file, input_start);
            }
            
            // Get output filename
            char *output_end = output_start;
            while (*output_end != '\0' && *output_end != ' ' && *output_end != '\t' && *output_end != '<') output_end++;
            *output_end = '\0';
            
            if (strlen(output_start) == 0)
            {
                printf(RED);
                printf("Error: syntax error, missing value after '>'\n");
                printf(RESET);
                return 0;
            }
            strcpy(output_file, output_start);
        }
        else
        {
            // Check for input redirection only
            redirect_pos = strchr(command_str, '<');
            if (redirect_pos != NULL)
            {
                *redirect_pos = '\0';
                strcpy(final_command, command_str);
                
                // Trim whitespace from command
                char *end = final_command + strlen(final_command) - 1;
                while (end > final_command && (*end == ' ' || *end == '\t')) end--;
                *(end + 1) = '\0';
                
                // Get input file
                char *input_start = redirect_pos + 1;
                while (*input_start == ' ' || *input_start == '\t') input_start++;
                
                char *input_end = input_start;
                while (*input_end != '\0' && *input_end != ' ' && *input_end != '\t') input_end++;
                *input_end = '\0';
                
                if (strlen(input_start) == 0)
                {
                    printf(RED);
                    printf("Error: syntax error, missing value after '<'\n");
                    printf(RESET);
                    return 0;
                }
                strcpy(input_file, input_start);
            }
        }
    }
    
    return 1;
}

void execute_single_pipe_command(char *command, char *home_dir, char *prev_dir)
{
    char cmd[PATH_MAX];
    char arg[20][PATH_MAX];
    int k = 0;
    
    // Tokenize the command
    char *token = strtok(command, " \t");
    if (token != NULL)
    {
        strcpy(cmd, token);
        token = strtok(NULL, " \t");
        while (token != NULL && k < 20)
        {
            strcpy(arg[k++], token);
            token = strtok(NULL, " \t");
        }
    }
    
    // Execute built-in commands
    if (strcmp(cmd, "hop") == 0)
        hop(arg, k, home_dir, prev_dir);
    else if (strcmp(cmd, "reveal") == 0)
        reveal(arg, k, home_dir, prev_dir);
    else if (strcmp(cmd, "proclore") == 0)
    {
        if (k == 0)
        {
            __pid_t pid = getpid();
            char pid_str[PATH_MAX];
            sprintf(pid_str, "%d", pid);
            print_proc_info(pid_str);
        }
        else if (k == 1)
        {
            print_proc_info(arg[0]);
        }
    }
    else if (strcmp(cmd, "seek") == 0)
        seek(arg, k, home_dir, prev_dir);
    else if (strcmp(cmd, "activities") == 0)
        activities();
    else if (strcmp(cmd, "ping") == 0)
        execute_ping(arg, k);
    else if (strcmp(cmd, "fg") == 0)
        fg(arg, k);
    else if (strcmp(cmd, "bg") == 0)
        bg(arg, k);
    else if (strcmp(cmd, "neonate") == 0)
        neonate(arg, k);
    else if (strcmp(cmd, "iMan") == 0)
    {
        if (k >= 1)
            man_page(cmd, arg);
        else
        {
            printf(RED);
            printf("Error: please provide details of which man page do you want?\n");
            printf(RESET);
        }
    }
    else if (strcmp(cmd, "") != 0)
    {
        // Execute external command
        char *args[22];
        args[0] = cmd;
        for (int i = 0; i < k; i++)
        {
            args[i + 1] = arg[i];
        }
        args[k + 1] = NULL;
        
        if (execvp(cmd, args) == -1)
        {
            printf(RED);
            printf("Error: Command '%s' not found\n", cmd);
            printf(RESET);
        }
    }
}