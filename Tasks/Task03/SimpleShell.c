#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define MAX_ARGS 100

void pwd();
void echo(char *args[]);
void exit_shell();
void help();
void mv(char *args[]);
int file_exist(char * file_name);

char* commands[] = {"pwd", "echo","exit","help","mv"};
int main(void)
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
		pwd();
	}
	else if(strcmp(commands[1],args[0])==0)
	{
		echo(args);
	}
	else if(strcmp(commands[2],args[0])==0)
	{
		
		exit_shell();
	}
	else if(strcmp(commands[3],args[0])==0)
	{
		help();
	}
	else if(strcmp(commands[4],args[0])==0)
	{

		mv(args);
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
		printf("getcwd");
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
		printf("Doesnt exist");

}

int file_exist(char * file_name)
{
	return access(file_name, F_OK)==0;
}
