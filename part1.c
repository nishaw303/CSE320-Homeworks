#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <signal.h>
#include <libgen.h>

char* commands[] = {"help", "run", "exit"};
char* const initial_prompt = "For help, type \"help\"\n";
char* const help_prompt = "To run an application, type \"run\" followed by the relative or absolute path or the filename of the application\nTo exit, type \"exit\"\n";
char input[255];

int main(int argc, char* argv[], char* envp[]){
    
    signal(SIGINT, SIG_IGN);
    pid_t pid;
    int status;
    char* appname = (char*)malloc(255);
    
    printf("%s", initial_prompt);
prompt:
    fgets(input, 255, stdin);
        
    char* command = strtok(input, " \n");
    
    if (!strcmp(command, commands[0])){
    	printf("%s", help_prompt);
    	goto prompt;
    }
    else if (!strcmp(command, commands[2])){
        printf("Exiting...\n");
        return 0;
    }
    else if (!strcmp(command, commands[1])){
    	pid = fork();
    	if (pid == 0){
    		strcpy(appname, strtok(NULL, " \n"));
    		
    		char* params[10];
    		params[0] = appname;
    		
    		int i;
    		for (i = 1; i < 10; i++){
    			params[i] = strtok(NULL, " \n");
    			if (!params[i]) break;
    		}
    		if (!(strchr(params[0], '/'))){
    			params[0] = realpath(appname, 0);
    		}
    		
    		execvp(params[0], &params[0]);
    		
    		printf("Error: application not found.\n");
    		return 1;
        }
        waitpid(pid, &status, 0);
        goto prompt;
    }
    else{
        printf("No valid command entered.\n");
        goto prompt;
	}
}
