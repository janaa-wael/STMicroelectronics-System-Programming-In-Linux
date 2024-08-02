#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

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
void type(const char * command);
int is_command_internal(const char* command_type);
int is_command_external(const char* command_type);
void print_env_var();
void execute_command(const char *command, char *const args[]);
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
	printf("GoodBye\n");
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


void mv(char* args[])
{
	/*int force = 0;

        if(strcmp(args[1], "-f") == 0)
        {
	        force = 1;
       		 args++;
        }

        if(args[1] == NULL || args[2] == NULL)
        {
                fprintf(stderr,"Usage : mv [-f] source_file target_file\n");
        	return;
	}
        char *source = args[1];
        char *target = args[2];
        char *target_path = strdup(target);

        if(access(target,F_OK) == 0 && access(target, X_OK) == 0)
        {
                target = strcat(target, "/");
                target = strcat(target, basename(source));
        }

        if(access(target, F_OK) ==0 && !force)
        {
                fprintf(stderr, "Error : Target file exists\n");
                free(target_path);
                return;
        }

        if(rename(source, target) < 0)
        {
                perror("rename");
        }

        free(target_path);*/
	int res = file_exist("Tst.c");
	if(res)
		printf("Exists");
	else
		printf("Doesnt exist\n");

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


void execute_command(const char *command, char *const args[]) {
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
