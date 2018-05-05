#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <signal.h>
#include <pthread.h>

/* Our commands */
char* commands[] = {"create", "kill", "list", "mem", "allocate", "read", "write", "exit"};

/* Our array of processes (threads) */
pthread_t threads[4] = {NULL};

/* Prompt to be printed */
char* const prompt_str = "prompt> ";

/* Input buffer */
char input[255];

/* Arrays of page tables */


void* cse320_virt_to_phys(){

}

void* thread_func(){
	while(1){
		sleep(1);
	}
}

int main(int argc, char* argv[], char* envp[]){
    
    pid_t pid;
    int status;
    int num_threads = 0;
    char* appname = (char*)malloc(255);
    
prompt:

    printf("%s", prompt_str);
    fgets(input, 255, stdin);
        
    char* command = strtok(input, " \n");
    
    /* Create */
    if (!strcmp(command, commands[0])){
    	if (num_threads == 4){
    		printf("Max number of processes reached\n");
    		goto prompt;
    	}
    	int i;
    	int t_return = -1;
    	for (i = 0; i < 4; i++){
    		if (!threads[i]){ 
    			t_return = pthread_create(&threads[i], NULL, thread_func, NULL);
    			break;
    		}
    	}
    	if (t_return != 0){
    		printf("Error creating thread\n");
    		goto prompt;
    	}
    	num_threads++;
    	goto prompt;
    }
    
    /* Kill */
    else if (!strcmp(command, commands[1])){
    	int i;
    	char* temp = strtok(NULL, " \n");
    	unsigned long thread_id = strtoul(temp, NULL, 10);
    	for (i = 0; i < 4; i++){
    	    if (threads[i] == thread_id){
    	        threads[i] = NULL;
    	        pthread_kill(&thread_id, SIGTERM);
    	        num_threads--;
    	        goto prompt;
    	    }
    	}
    	printf("Process ID not found\n");
        goto prompt;
    }
    
    /* List */
    else if (!strcmp(command, commands[2])){
    	int i;
    	if (num_threads == 0){
    	    printf("No current processes\n");
    	    goto prompt;
    	}
    	for (i = 0; i < 4; i++){
    		if (threads[i]){
    			printf("Process %d with ID: %lu\n", i, (unsigned long)threads[i]);
    		}
    	}
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
    
    /* Exit */
    else if (!strcmp(command, commands[7])){
    	
        return 0;
    }
    
    /* Inalid command */
    else{
        printf("Invalid command\n");
        goto prompt;
	}
}
