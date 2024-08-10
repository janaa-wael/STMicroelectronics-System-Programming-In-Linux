#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include "SimpleShell.h"
char* commands[] = {"mypwd", "myecho","myexit","myhelp","mymv","mycp","mycd","mytype","myenv","free","uptime"};

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

	readSize = read(STDIN, command, sizeof(command) - 1);
	
	if(readSize <= 0)
	{
		perror("read");
		continue;
	}
	command[readSize] = '\0';
	if(readSize > 0 && command[readSize - 1] == '\n')
	{
		command[readSize - 1] = '\0';
	}
	
	/* get the first token */
	args[arg_count] = strtok(command," ");
	
	while(args[arg_count] != NULL && arg_count < MAX_ARGS - 1)
	{
		arg_count++;
		args[arg_count] = strtok(NULL, " ");
	}

	args[arg_count] = NULL;


	int input_fd = STDIN_FILENO;
	int output_fd = STDOUT_FILENO;
	int error_fd = STDERR_FILENO;

	for(int i = 0 ; args[i] != NULL; i++)
	{
		if(strcmp(args[i],"<") == 0)
		{
			input_fd = open(args[i+1], O_RDONLY);
			if(input_fd < 0)
			{
				perror("open input files");
				input_fd = STDIN_FILENO;
			}
			args[i] = NULL; //remove redirection part from args
		} else if(strcmp(args[i], ">")== 0)
		{
			output_fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if(output_fd < 0)
			{
				perror("open output file");
				output_fd = STDOUT_FILENO;
			}
			args[i] = NULL;
		}
		else if (strcmp(args[i], "2>") == 0)
		{
			error_fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if(error_fd < 0)
			{
				perror("open error file");
				error_fd = STDERR_FILENO;
			}
			args[i] = NULL;
		}
	}

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
	else if(strcmp(commands[9],args[0]) ==0)
	{
		print_memory_info();
	}
	else if(strcmp(commands[10],args[0])==0)
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
	       execute_command(args[0],args,input_fd,output_fd,error_fd);	
	
	}
		input_fd = STDIN_FILENO;
		output_fd = STDOUT_FILENO;
		error_fd = STDERR_FILENO;
		
	}
	return 0;
}

void pwd()
{
	char path[1024];
	if(getcwd(path,sizeof(path)) != NULL)		
	{
		printf("Path: %s\n",path);
	}
	else
	{
		printf("getcwd\n");
	}
}

void echo(char* args[])
{

	int i =1;
	while(args[i] != NULL){
            	 printf("%s ",args[i]);
		 i++;
        }
	printf("\n");

}

void exit_shell()
{
	printf("GoodBye%c\n",7);
	exit(1);

}


void help()
{
	printf("Supported Commands:\n");
	printf("~~~~~~~~~~~~~~~~~~~\n");
	printf("1- Pwd : Prints the current path directory\n");
	printf("2- Echo : Displaying lines of text passed as argument to the command line\n");
	printf("3- Cp : Copies file to another file\n");
	printf("4- Mv : Move file from one directory to another directory.\n");

}


int file_exist(char * file_name)
{
	return access(file_name, F_OK)==0;
}


void copy(char * sourcepath, char * targetpath, char flag)
{
	int srcFd = open(sourcepath, O_RDONLY);
	if(access(targetpath, F_OK) == 0)
	{
		fprintf(stderr, "Error! Target file already exists!\n", targetpath);
		close(srcFd);
		return;
	}
	int trgFd = open(targetpath, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(trgFd < 0)
	{
		perror("Error opening the target file\n");
		close(srcFd);
		return;
	}
	char buff[BUFF_SIZE];
	ssize_t bytesRead, bytesWritten;

	while((bytesRead = read(srcFd, buff, BUFF_SIZE)) > 0)
	{
		bytesWritten = write(trgFd, buff, bytesRead);
		if(bytesWritten != bytesRead)
		{
			perror("Error writing to target file\n");
			close(srcFd);
			close(trgFd);
			return;
		}
	}

	if(bytesRead < 0)
	{
		perror("Error reading from source file\n");
	}

	close(srcFd);
	close(trgFd);

	return 0;
}

void move(char* sourcepath, char* targetpath)
{

	if(access(sourcepath, F_OK) != 0)
	{
		perror("Error! Source Path doesn't exist!\n");
		return;
	}
	 
  	  int size = strlen(sourcepath);
  	  char temp[50];
  	  int i = size-1;
  	  int counter = 0;
  	  while(sourcepath[i] != '/' && i>=0)
  	  {
  	       i--;
  	  }
	  int j;
	  for( j = 0 ; j < size-i ; j++)
	  {
		temp[j] = sourcepath[i++];
	  }
  	  temp[j] = '\0';
  	  strcat(targetpath,temp);
	char *filepath = sourcepath;
	copy(filepath, targetpath, 1);
	if(unlink(filepath) != 0)
	{
		perror("Error moving file\n");
		return;
	}
	printf("%s moved successfully! :)\n",sourcepath);
	return;
}

void cd(char* new_dir)
{
	if(chdir(new_dir)!=0)
	{
		perror("chdir");
		return ;
	}

	char currentDir[1000];
	if(getcwd(currentDir,sizeof(currentDir)) != NULL)
	{
		printf("Current Working Directory: %s\n",currentDir);
	}
	else
	{
		perror("getcwd");
	
	}
	return;
}

void type(const char * command)
{
	if(is_command_internal(command))
	{
		printf("Internal Command\n");
	}
	else if(is_command_external(command))
	{
		printf("External Command\n");
	}
	else
	{
		printf("The command is unsupported");
	}
}

int is_command_internal(const char* command_type)
{	
	for(int i = 0 ; i < sizeof(commands)/sizeof(char *) ; i++)
	{
		
		if(strcmp(command_type,commands[i])==0)
		{
			return 1;
		}
	}
	return 0;
}

int is_command_external(const char* command_type)
{
	char * path_env = getenv("PATH");
	printf("Here 1\n");
	if(path_env == NULL)
	{

		printf("Here 12\\n");
		perror("strdup");
		return 0;
	}
	printf("Here 22");
	char path[1024];
	char *path_copy = strdup(path_env);
	printf("Here 2\n");
	if(path_copy == NULL)
	{
		perror("strdup");
		return 0;
	}
	printf("Here 3\n");
	char *dir = strtok(path_copy, ":");
	while(dir != NULL)
	{
		printf("Here 4\n");
		snprintf(path, sizeof(path), "%s/%s", dir, command_type);
		if(access(path, X_OK) == 0)
		{
			free(path_copy);
			return 1;
		}
		dir = strtok(NULL, ":");
	}
	printf("Here 5");
	free(path_copy);
	return 0;
}

void print_env_var()
{
	extern char **environ; //ptr to env variables
	char **current = environ; //saart of the env var list
	//Iterate through the env variables
	while(*current != NULL)
	{
		printf("%s\n", *current);
		current++;
	}

}


void execute_commands(const char *command, char *const args[]) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        // Execute the command
        execvp(command, args);
        // If execvp returns, an error occurred
        perror("execvp");
        exit(EXIT_FAILURE);
    } else { // Parent process
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
        }
        if (WIFEXITED(status)) {
            printf("Child exited with status %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child terminated by signal %d\n", WTERMSIG(status));
        }
    }
}

void print_memory_info()
{
	FILE *file;
	char buffer[BUFF_SIZE];
	unsigned long total_ram , free_ram , total_swap, free_swap;

	file = fopen("/proc/meminfo", "r");

	if(file == NULL)
	{
		perror("fopen");
		return;
	}

	while(fgets(buffer, sizeof(buffer), file))
	{
		if(strncmp(buffer, "MemTotal:", 9) == 0)
			sscanf(buffer, "MemTotal: %lu kB", &total_ram);
		else if(strncmp(buffer, "MemTotal:", 8) == 0)
		{
			sscanf(buffer, "MemFree: %lu kB", &free_ram);
				
		}
		else if(strncmp(buffer, "SwapTotal:", 10) ==0)
		{
			sscanf(buffer, "SwapTotal: %lu", &total_swap);
		}
		else if(strncmp(buffer, "SwapFree:", 9) == 0)
		{
			sscanf(buffer, "SwapFree: %lu kB", &free_swap);
		}
	}

	fclose(file);

	printf("RAM Info:\n");
	printf("Total RAM: %lu kB\n",total_ram);
	printf("Free RAM: %lu kB\n", free_ram);
	printf("Used RAM: %lu kB\n",total_ram - free_ram);
	
	printf("Swap Info:\n");
        printf("Total Swap: %lu kB\n",total_swap);
        printf("Free Swap: %lu kB\n", free_swap);
        printf("Used Swap: %lu kB\n",total_swap - free_swap);

}

void print_uptime_info()
{
	FILE * file;
	double uptime, idle_time;

	file = fopen("/proc/uptime", "r");
	if(file == NULL)
	{
		perror("fopen");
		return;
	}

	if(fscanf(file, "%lf %lf", &uptime, &idle_time) != 2)
	{
		perror("fscanf");
		fclose(file);
		return;
	}

	fclose(file);

	printf("System Uptime: %.2f seconds\n",uptime);
	printf("Idle Time: %.2f second\n",idle_time);
}

void execute_command(const char* command, char* const args[], int input_fd, int output_fd, int error_fd)
{
	pid_t pid = fork();
	if(pid < 0)
	{
		perror("Fork");
		exit(EXIT_FAILURE);
	}

	if(pid == 0)
	{
		if(input_fd != STDIN_FILENO)
		{
			dup2(input_fd, STDIN_FILENO);
			close(input_fd);
		}

		if(output_fd != STDOUT_FILENO)
		{
			dup2(output_fd, STDOUT_FILENO);
			close(output_fd);
		}

		if(error_fd != STDERR_FILENO)
		{
			dup2(error_fd, STDERR_FILENO);
			close(error_fd);
		}	

		execvp(command,args);
		perror("execvp");
		exit(EXIT_FAILURE);

	}
	else 
	{
		wait(NULL);
	}
}
