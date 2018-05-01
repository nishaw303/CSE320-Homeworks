#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <signal.h>
#include <libgen.h>

/* Our commands */
char* commands[] = {"create", "kill", "list", "mem", "allocate", "read", "write", "exit"};

/* Our array of processes (pids) */
pid_t pids[4];

/* Prompt to be printed */
char* const prompt_str = "prompt> ";

/* Input buffer */
char input[255];

int main(int argc, char* argv[], char* envp[]){
    
    pid_t pid;
    int status;
    char* appname = (char*)malloc(255);
    
prompt:

    printf("%s", prompt_str);
    fgets(input, 255, stdin);
        
    char* command = strtok(input, " \n");
    
    /* Exit */
    if (!strcmp(command, commands[7])){
    	
        return 0;
    }
    /* Create */
    else if (!strcmp(command, commands[0])){
    
    	goto prompt;
    }
    /* Kill */
    else if (!strcmp(command, commands[1])){
    
        goto prompt;
    }
    /* List */
    else if (!strcmp(command, commands[2])){
    
    	goto prompt;
    }
    /* Mem */
    else if (!strcmp(command, commands[3])){
    
    	goto prompt;
    }
    /* Allocate */
    else if (!strcmp(command, commands[4])){
    
    	goto prompt;
    }
    /* Read */
    else if (!strcmp(command, commands[5])){
    
    	goto prompt;
    }
    /* Write */
    else if (!strcmp(command, commands[6])){
    
    	goto prompt;
    }
    /* Inalid command */
    else{
        printf("Invalid command\n");
        goto prompt;
	}
}
