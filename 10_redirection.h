#ifndef REDIRECTION_H
#define REDIRECTION_H

extern int restore_flag;
extern int input_fd;
extern int output_fd;

void tokenise_space(char *input, char command[PATH_MAX], char arg[20][PATH_MAX], int *flag_log, int *k);
int tokenise_and_execute_redirection(char *token_ampersand, int cnt_less_than, int cnt_greater_than, int cnt_2greater_than,int pos_less,int pos_greater,int pos_2greater,int *back_ground_no, int *flag_log,char* home_dir,char* prev_dir);
void redirect_in_fg(char* command,char arg[20][4096], char* input_file,int k,char* home_dir,char* prev_dir);
void redirect_in_bg(char* command,char arg[20][4096], char* input_file,int k,char* home_dir,char* prev_dir);
void redirect_out_fg(char* command,char arg[20][4096], char* output_file,int k,char* home_dir,char* prev_dir);
void redirect_out_bg(char* command,char arg[20][4096], char* output_file,int k,char* home_dir,char* prev_dir);
void redirect_out2_fg(char* command,char arg[20][4096], char* output_file,int k,char* home_dir,char* prev_dir);
void redirect_out2_bg(char* command,char arg[20][4096], char* output_file,int k,char* home_dir,char* prev_dir);
void redirect_inout_fg(char* command,char arg[20][4096],char* input_file, char* output_file,int k,char* home_dir,char* prev_dir);
void redirect_inout_bg(char* command,char arg[20][4096],char* input_file, char* output_file,int k,char* home_dir,char* prev_dir);
void redirect_inout2_fg(char* command,char arg[20][4096],char* input_file, char* output_file,int k,char* home_dir,char* prev_dir);
void redirect_inout2_bg(char* command,char arg[20][4096],char* input_file, char* output_file,int k,char* home_dir,char* prev_dir);


#endif