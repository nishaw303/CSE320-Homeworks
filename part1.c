#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char* commands[] = {"help", "run", "exit"};
char* const initial_prompt = "For help, type \"help\"\n";
char* const help_prompt = "To run an application, type \"run\" followed by the relative or absolute path to the application\nTo exit, type \"exit\"\n";

int main(int argc, char* argv[], char* envp[]){
    
    pid_t pid;
    int status;
    char* appname = (char*)malloc(255);
    appname = argv[1];
    char* input;
    
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
    	
    		/* put code to run application here */
    		
    		printf("Error: should not return.\n");
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
