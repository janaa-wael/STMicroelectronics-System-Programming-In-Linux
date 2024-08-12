#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include "SimpleShell.h"

const char* commands[] = { "mypwd", "myecho","myexit","myhelp","mymv","mycp","mycd","mytype","myenv","free","uptime" };


int min_valid_arguments[] = { 1, 2, 1, 1, 3, 3 };

int main(int argc, char** argv)
{
	char command[PATH_MAX];
	char* args[MAX_ARGS];
	ssize_t readSize = 0;
	int arg_count = 0;
	int output_redirection_flag = 0,error_redirection_flag = 0,input_redirection_flag = 0;
	const char* shellMsg = "O2mor Ya Ghaly >> ";
	while (1) {
		memset(command, 0, sizeof(command));
		
		write(STDOUT, shellMsg, strlen(shellMsg));

		readSize = read(STDIN, command, sizeof(command) - 1);

		if (readSize <= 0)
		{
			perror("read");
			continue;
		}
		command[readSize] = '\0';
		if (readSize > 0 && command[readSize - 1] == '\n')
		{
			command[readSize - 1] = '\0';
		}

		/* get the first token */
		args[arg_count] = strtok(command, " ");
		printf("t");
		redirection_flag = 0;
		while (args[arg_count] != NULL && arg_count < MAX_ARGS - 1)
		{
			if(strcmp(args[arg_count],">") == 0) 
				output_redirection_flag = 1;
			else if(strcmp(args[arg_count],"<") ==0)
				input_redirection_flag = 1;
			else if(strcmp(args[arg_count],"2>") == 0)
				error_redirection_flag = 1;
			arg_count++;
			args[arg_count] = strtok(NULL, " ");
			
		}

		args[arg_count] = NULL;
		
		printf("redirect = %d\n",redirection_flag);
		/* walk through other tokens */
		if(redirection_flag == 0)
		{
			if (strcmp(commands[0], args[0]) == 0)
			{
				if (min_valid_arguments[0] >= arg_count + 1)
				{
					perror("Insufficient Arguments!");
					continue;
				}

				pwd();
			}
			else if (strcmp(commands[1], args[0]) == 0)
			{
				if (min_valid_arguments[1] >= arg_count + 1)
				{
					perror("Insufficient Arguments!");
					continue;
				}
				echo(args);
			}
			else if (strcmp(commands[2], args[0]) == 0)
			{
				if (min_valid_arguments[2] >= arg_count + 1)
				{
					perror("Insufficient Arguments!");
					continue;
				}
				exit_shell();
			}
			else if (strcmp(commands[3], args[0]) == 0)
			{
				if (min_valid_arguments[3] >= arg_count + 1)
				{
					perror("Insufficient Arguments!");
					continue;
				}

				help();
			}
			else if (strcmp(commands[4], args[0]) == 0)
			{
				if (min_valid_arguments[4] >= arg_count + 1)
				{
					perror("Insufficient Arguments!");
					continue;
				}

				move(args[1], args[2]);

			}
			else if (strcmp(commands[5], args[0]) == 0)
			{
				if (min_valid_arguments[5] >= arg_count + 1)
				{
					perror("Insufficient Arguments!");
					continue;
				}

				copy(args[1], args[2], 0);
			}
			else if (strcmp(commands[6], args[0]) == 0)
			{
				cd(args[1]);
			}
			else if (strcmp(commands[7], args[0]) == 0)
			{
				type(args[1]);
			}
			else if (strcmp(commands[8], args[0]) == 0)
			{
				print_env_var();
			}
			else if (strcmp(commands[9], args[0]) == 0)
			{
				print_memory_info();
			}
			else if (strcmp(commands[10], args[0]) == 0)
			{
				print_uptime_info();
			}
			else if(is_command_external(args[0]))
			{
			
				execute_external_command(args[0], args);
			}
		}
		
		else if(redirection_flag == 1)
		{
			if(output_redirection_flag == 1)
				output_redirection(args,arg_count);
			else if(input_redirection_flag == 1)
			
			else if(error_redirection_flag == 1)
				error_redirection(args,args[0]);
		}
		fflush(stdout);
		fflush(stdin);
	
	}
	return 0;
}
