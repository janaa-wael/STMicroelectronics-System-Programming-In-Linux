#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include "SimpleShell.h"
char* commands[] = { "mypwd", "myecho","myexit","myhelp","mymv","mycp","mycd","mytype","myenv","free","uptime" };

int min_valid_arguments[] = { 1, 2, 1, 1, 3, 3 };

int main(int argc, char** argv)
{
	char command[PATH_MAX];
	char* args[MAX_ARGS];
	ssize_t readSize = 0;
	int arg_count = 0;

	const char* shellMsg = "O2mor Ya Ghaly >> ";
	while (1) {
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

		while (args[arg_count] != NULL && arg_count < MAX_ARGS - 1)
		{
			arg_count++;
			args[arg_count] = strtok(NULL, " ");
		}

		args[arg_count] = NULL;


		int input_fd = STDIN_FILENO;
		int output_fd = STDOUT_FILENO;
		int error_fd = STDERR_FILENO;

		for (int i = 0; args[i] != NULL; i++)
		{
			if (strcmp(args[i], "<") == 0)
			{
				input_fd = open(args[i + 1], O_RDONLY);
				if (input_fd < 0)
				{
					perror("open input files");
					input_fd = STDIN_FILENO;
				}
				args[i] = NULL; //remove redirection part from args
			}
			else if (strcmp(args[i], ">") == 0)
			{
				output_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if (output_fd < 0)
				{
					perror("open output file");
					output_fd = STDOUT_FILENO;
				}
				args[i] = NULL;
			}
			else if (strcmp(args[i], "2>") == 0)
			{
				error_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if (error_fd < 0)
				{
					perror("open error file");
					error_fd = STDERR_FILENO;
				}
				args[i] = NULL;
			}
		}

		/* walk through other tokens */
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
		else
		{
			/*char* command = args[1];
			char **args_ = malloc((arg_count) * sizeof(char *));
			if (args_ == NULL)
			{
				perror("malloc");
				exit(EXIT_FAILURE);
			}

			args_[0] = command;
			for(int i = 2; i < arg_count; i++)
			{
				args_[i-1] = args[i];
			}
			args_[arg_count*/
			execute_command(args[0], args, input_fd, output_fd, error_fd);

		}
		input_fd = STDIN_FILENO;
		output_fd = STDOUT_FILENO;
		error_fd = STDERR_FILENO;

	}
	return 0;
}