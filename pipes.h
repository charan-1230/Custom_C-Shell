#ifndef PIPES_H
#define PIPES_H

int tokenise_and_execute_pipe(char *token_ampersand, int *flag_log, char *home_dir, char *prev_dir);
void execute_pipe_commands(char commands[][PATH_MAX], int num_commands, char *home_dir, char *prev_dir);
int parse_pipe_with_redirection(char *command_str, char *final_command, char *input_file, char *output_file, int *append_mode);
void execute_single_pipe_command(char *command, char *home_dir, char *prev_dir);

#endif