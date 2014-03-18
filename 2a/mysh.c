#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h> 
#include <ctype.h>
#include <string.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>

int batchMode;
int argcount;
char *outFile;
int redirect;
FILE *batchfd;

int
redirectOutput(char *inputLine) {
	if (strchr(inputLine, '>') != NULL){
		int i = 0;
		char *a = strtok(inputLine, ">\n\t");
		outFile = "";
		while(a != NULL){
			if (i > 1) 
				return -1;
			if (i == 1) 
				outFile = strdup(a);
			a = strtok(NULL, " >\n\t");
			if (i == 0 && a == NULL)
				return -1;
			i++;
		} 
			return 1;
	}
	else {
		return 0;
	}
}

char **
	prompt()
{
	char input[512];
	char *prompt = "mysh>";
	char *returnValue = NULL;

	if (batchMode == 0){
	  write(STDOUT_FILENO, prompt, strlen(prompt));
	  returnValue = fgets(input, 512, stdin);
	}
	else
	{
	  returnValue = fgets(input,512,batchfd);
          if (returnValue == NULL) exit(0);	  
	  write(STDOUT_FILENO, returnValue, strlen(returnValue));
	}

	char *inputLine = strdup(returnValue);
	redirect = redirectOutput(inputLine);
	if (redirect == 1)
		inputLine = strtok(returnValue, ">");
	if (returnValue == NULL)
	{
		return NULL;
	}
	char **tokens;
	int i = 0;
	tokens = (char **) malloc(sizeof(char*));
	char *currToken = strtok(inputLine, " \n\t");
	while (currToken != NULL)
	{
		tokens = (char **) realloc(tokens, (i + 1)*sizeof(char*));
		tokens[i] = strdup(currToken);
		i++;
		currToken = strtok(NULL, " \n\t");
	}
	argcount = i;
	return tokens;
}

void
	error()
{
	char error_message[30] = "An error has occured\n";
	write(STDERR_FILENO, error_message,strlen(error_message));
}
/*
void
	usage()
{
	char *err = "Usage: myshell\n";
	write(STDERR_FILENO, err, strlen(err));
	exit(1);
}*/
void
runProcess(char **arguments) {
	if (arguments != NULL) {
		if (strcmp(arguments[0],"exit") == 0) {
			exit(0);
		} else if (strcmp(arguments[0],"cd") == 0) {
			if (argcount == 2)
				chdir(arguments[1]);
			else if(argcount == 1)
				chdir(getenv("HOME"));
			else
				error();
		}
		else {
			int rc = fork();
			if (rc == 0) { //CHILD process
				//int ro = redirectOutput(inputLine);
				char *file = strdup(arguments[0]);
				if (redirect == -1) {
					error();
					exit(1);
				} else if (redirect == 1) {
					//redirect = 0;							
					close(STDOUT_FILENO); 
					open(outFile,O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
				}
				if (strstr(file,".c") != NULL) {
					char *program = strtok(file,".");
					char *array[5];
					write(STDOUT_FILENO, program,strlen(program));
					array[0] = "gcc";
					array[1] = "-o";
					array[2] = strdup(program);
					sprintf(array[3],"%s.c",program);
					array[4] = "-Wall";
					write(STDOUT_FILENO, array[0], 4);
					int i;
					char *newArguments[argcount];

					sprintf(newArguments[0],"./%s",program);
					
					write(STDOUT_FILENO, "before for\n", 11);	
					for (i = 1; i < argcount; i++)
						newArguments[i] = strdup(arguments[i]);
					int rc1 = fork();
					write(STDOUT_FILENO,"before\n",8);
					if (rc1 == 0){
						write(STDOUT_FILENO, "here\n", 6);
						execvp(array[0],array);
						error();
					}
					else if (rc1>  0)
					{
						(void)wait(0);
						execvp(newArguments[0],newArguments);
						error();
					}
						write(STDOUT_FILENO,"whoops\n", 8);
						exit(0);
					}
					if (strcmp(arguments[0],"pwd") == 0) {
						size_t size = PATH_MAX;
						char path[PATH_MAX];
						char *string = getcwd(path,size);
						string = strcat(string,"\n");
						write(STDOUT_FILENO, string,strlen(string));
						exit(0);
					} else {
						execvp(arguments[0], arguments);
						error();
						exit(1);
					}
				} else if (rc > 0) {
				(void)wait(0);
				free(*arguments);
			} else {
	
			}
		}
	}
}

int
	main(int argc, char *argv[]){
		char **arguments;

		if (argc > 1) {
			batchMode = 1;
			batchfd = fopen(argv[1],"r");
			if (batchfd != NULL) {
				while (1) 
				{
					arguments = prompt();
					runProcess(arguments);
				}
				exit(0);		
			}
			else {
				error();
				exit(1);
			}
			
		}else{
			while(1)
			{
				arguments = prompt();
				runProcess(arguments);
			}
}}
