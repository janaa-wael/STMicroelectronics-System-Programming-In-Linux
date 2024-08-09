#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include "SimpleShell.h"
char* commands[] = {"mypwd", "myecho","myexit","myhelp","mymv","mycp","mycd","mytype","myenv"};
int min_valid_arguments[] = {1, 2, 1, 1, 3, 3};

int main(int argc, char ** argv)
{
	char command[PATH_MAX];
	char *args[MAX_ARGS];
	ssize_t readSize = 0;
	int arg_count = 0;

	const char * shellMsg = "O2mor Ya Ghaly >> ";
	while(1){
	write(STDOUT, shellMsg, strlen(shellMsg));

	readSize = read(STDIN, command, 100);
	
	command[readSize] = '\0';

	/* get the first token */
	args[arg_count] = strtok(command," ");
	
	while(args[arg_count] != NULL && arg_count < MAX_ARGS - 1)
	{
		arg_count++;
		args[arg_count] = strtok(NULL, " ");
	}
	args[arg_count] = NULL;

	/* walk through other tokens */
	if(strcmp(commands[0],args[0])==0)
	{
		if(min_valid_arguments[0] >= arg_count+1)
                {
                        perror("Insufficient Arguments!");
                        continue;
                }

		pwd();
	}
	else if(strcmp(commands[1],args[0])==0)
	{
		if(min_valid_arguments[1] >= arg_count+1)
		{
			perror("Insufficient Arguments!");
			continue;
		}
		echo(args);
	}
	else if(strcmp(commands[2],args[0])==0)
	{
		if(min_valid_arguments[2] >= arg_count+1)
                {
                        perror("Insufficient Arguments!");
                        continue;
                }
		exit_shell();
	}
	else if(strcmp(commands[3],args[0])==0)
	{
		if(min_valid_arguments[3] >= arg_count+1)
                {
                        perror("Insufficient Arguments!");
                        continue;
                }

		help();
	}
	else if(strcmp(commands[4],args[0])==0)
	{
		if(min_valid_arguments[4] >= arg_count+1)
                {
                        perror("Insufficient Arguments!");
                        continue;
                }

		move(args[1],args[2]);

	}
	else if(strcmp(commands[5],args[0])==0)
	{
		if(min_valid_arguments[5] >= arg_count+1)
                {
                        perror("Insufficient Arguments!");
                        continue;
                }

		copy(args[1],args[2],0);
	}
	else if(strcmp(commands[6],args[0])==0)
	{
		cd(args[1]);
	}
	else if(strcmp(commands[7],args[0])==0)
	{
		type(args[1]);
	}
	else if(strcmp(commands[8],args[0])==0)
	{
		print_env_var();
	}
	else 
	{
		char* command = args[1];
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
		args_[arg_count - 1] = NULL;

		execute_command(command, args_);
		free(args_);
	
	}
	}
	return 0;
}