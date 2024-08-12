#ifndef SIMPLE_SHELL_H
#define SIMPLE_SHELL_H

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define MAX_ARGS 100
#define BUFF_SIZE 200


void pwd();
void echo(char *args[]);
void exit_shell();
void help();
void mv(char *args[]);
int file_exist(char * file_name);
void copy(char * sourcepath, char * targetpath, char flag);
void move(char* sourcepath, char* targetpath);
void cd(char* new_dir);
void type(char * command);
int is_command_internal(char* command_type);
int is_command_external(char* command_type);
void print_env_var();
void execute_commandd(const char *command, char *const args[]);
void print_memory_info();
void print_uptime_info();
int isThereRedirection(char* command);
void execute_external_command(const char* command, char* const args[]);
void error_redirection(char *args[], char *command);
void input_redirection(char *args[], char *command);
void pipe_with_grep(const char *cmd1, const char *cmd2, const char *search_term);
#endif
