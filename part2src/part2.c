#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char* commands[] = {"quit", "change", "solver", "trace", "fib"};
char* const text_prompt = "To choose an application to run, type either 'solver', 'trace', or 'fib'.\nTo exit, type 'quit'.\nTo change file being accessed, type 'change' followed by the file path.\n";
char input[255];

int main(int argc, char* argv[], char* envp[]){
    
    pid_t pid;
    int status;
    char* filename = (char*)malloc(255);
    filename = argv[1];
    
prompt:
    printf("%s", text_prompt);
    fgets(input, 255, stdin);
        
    char* command = strtok(input, " \n");
        
    if (!strcmp(command, commands[0])){
        printf("Exiting...\n");
        return 0;
    }
    else if (!strcmp(command, commands[1])){
    	strcpy(filename, strtok(NULL, " \n"));
        printf("Changing filepath to: %s\n", filename);
        goto prompt;
    }
    else if (!strcmp(command, commands[2]) || !strcmp(command, commands[3]) || !strcmp(command, commands[4])){
    	if (filename == NULL || fopen(filename, "r") || strstr(input, "fib") != NULL){
            pid = fork();
            if (pid == 0){
                if(strstr(input, "trace") != NULL){
                	char* const params[] = {"trace", filename, 0};
                    execve(params[0], &params[0], envp);
                }
                else if (strstr(input, "solver") != NULL){
                    char* const params[] = {"solver", filename, 0};
                    execve(params[0], &params[0], envp);
                }
                else{
                    char* const params[] = {"fib", filename, 0};
                    execve(params[0], &params[0], envp);
                }
                printf("Error: should not return.\n");
                return 1;
            }
            waitpid(pid, &status, 0);
            goto prompt;
        }
        else{
            printf("Error: file does not exist.\n");
            return 1;
        }
    }
    else{
        printf("No valid command entered.\n");
        goto prompt;
	}
}
