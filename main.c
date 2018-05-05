#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

/* Our commands */
char* commands[] = {"create", "kill", "list", "mem", "allocate", "read", "write", "exit"};

/* Our array of processes (threads) */
pthread_t threads[4] = {NULL};

/* Prompt to be printed */
char* const prompt_str = "prompt> ";

/* Input buffer */
char input[255];

/* Structs for page tables */
struct second_level_pt{
    int virt_addr[32];
};

struct first_level_pt{
    struct second_level_pt* ptr[8];
};

/* Actual page tables for "processes" */
struct first_level_pt* pageTables[4];

int cse320_malloc(unsigned long thread_id){
    int i;
    for (i = 0; i < 4; i++){
        if ((unsigned long)threads[i] == thread_id){
            break;
        }
    }
    if (pageTables[i] == NULL){
        pageTables[i] = (struct first_level_pt*)malloc(sizeof(struct first_level_pt));
        int j, k;
        for (j = 0; j < 8; j++){
            pageTables[i]->ptr[j] = (struct second_level_pt*)malloc(sizeof(struct second_level_pt));
            for (k = 0; k < 32; k++){
                pageTables[i]->ptr[j]->virt_addr[k] = 0;
            }
        }
        pageTables[i]->ptr[0]->virt_addr[0] = i*256;
        return i*256;
    }
    else {
        int j, k;
        for (j = 0; j < 8; j++){
            for (k = 0; k < 32; k++){
                if (pageTables[i]->ptr[j]->virt_addr[k] == 0 && (k && !j && !i)){
                    pageTables[i]->ptr[j]->virt_addr[k] = i*256+k;
                    return i*256+k;
                }
            }
        }
        return -1;
    }
}

int cse320_virt_to_phys(char* virtual_addr, int proc){
    char firstLvl[10];
    memcpy(firstLvl, virtual_addr, 10);
    virtual_addr+=10;
    char secondLvl[10];
    memcpy(secondLvl, virtual_addr, 10);
    int firstInd = strtoul(firstLvl, NULL, 2);
    int secondInd = strtoul(secondLvl, NULL, 2);
    if ((firstInd > 8) || (secondInd > 32)) return -1;
    if ((pageTables[proc] == NULL) || (!pageTables[proc]->ptr[firstInd]->virt_addr[secondInd] && proc)){
        return -1;
    }
    return pageTables[proc]->ptr[firstInd]->virt_addr[secondInd];
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
    char* readfifo = "/tmp/readfifo";
    char* writefifo = "/tmp/writefifo";
    mkfifo(readfifo, 'r');
    mkfifo(writefifo, 'w');
    
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
    	if (num_threads == 0){
    	    printf("No processes currently running\n");
    	    goto prompt;
    	}
    	char* temp = strtok(NULL, " \n");
    	unsigned long thread_id = strtoul(temp, NULL, 10);
    	for (i = 0; i < 4; i++){
    	    if ((unsigned long)threads[i] == thread_id){
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
        char* temp = strtok(NULL, " \n");
        if (temp == NULL){
            printf("Invalid input\n");
            goto prompt;   
        }
        unsigned long thread_id = strtoul(temp, NULL, 10);
        int i;
        for (i = 0; i < 4; i++){
            if (threads[i] == thread_id){
                break;
            }
        }
        if (pageTables[i] == NULL){
            printf("No virtual memory allocated for process %lu\n", thread_id);
        }
        else{
            int j;
            for (j = 0; j < 8; j++){
                int k;
                for (k = 0; k < 32; k++){
                    if (!k && !i && !j){
                        printf("Virtual memory address: %d\n", pageTables[i]->ptr[j]->virt_addr[k]);
                    }
                    if (pageTables[i]->ptr[j]->virt_addr[k]){
                        printf("Virtual memory address: %d\n", pageTables[i]->ptr[j]->virt_addr[k]);
                    }
                }
            }
        }
    	goto prompt;
    }
    
    /* Allocate */
    else if (!strcmp(command, commands[4])){
        char* temp = strtok(NULL, " \n");
        if (temp == NULL){
            printf("Invalid input\n");
            goto prompt;   
        }
        unsigned long thread_id = strtoul(temp, NULL, 10);
        int out = cse320_malloc(thread_id);
        if (out == -1){
            printf("Exceeds max virtual memory allocation\n");
            goto prompt;
        }
        printf("Successfully allocated virtual memory at address: %d\n", out);
    	goto prompt;
    }
    
    /* Read */
    else if (!strcmp(command, commands[5])){
        char* temp = strtok(NULL, " \n");
        if (temp == NULL){
            printf("Invalid input\n");
            goto prompt;   
        }
    	unsigned long thread_id = strtoul(temp, NULL, 10);
    	
    	/* Finds which thread we are checking the memory for */
    	int i;
    	for (i = 0; i < 4; i++){
    	    if (thread_id == (unsigned long)threads[i]){
    	        break;
    	    }
    	}
    	int addr = cse320_virt_to_phys(strtok(NULL, " \n"), i);
    	if (addr < 0){
    	    printf("Address out of range\n", addr);
    	}
    	else if (addr % 4 != 0){
    	    printf("Address is not aligned\n", addr);
    	}
    	else{
            char buff[1024];
    	    int fd = open(readfifo, O_RDONLY);
    	    read(fd, buff, 4);
    	    printf("Integer at address %d: %d\n", addr, buff);
    	    close(fd);
    	}
    	goto prompt;
    }
    
    /* Write */
    else if (!strcmp(command, commands[6])){
        char* temp = strtok(NULL, " \n");
        if (temp == NULL){
            printf("Invalid input\n");
            goto prompt;   
        }
    	unsigned long thread_id = strtoul(temp, NULL, 10);
    	
    	/* Finds which thread we are checking the memory for */
    	int i;
    	for (i = 0; i < 4; i++){
    	    if (thread_id == (unsigned long)threads[i]){
    	        break;
    	    }
    	}
    	int addr = cse320_virt_to_phys(strtok(NULL, " \n"), i);
    	if (addr < 0){
    	    printf("Address out of range\n", addr);
    	}
    	else if (addr % 4 != 0){
    	    printf("Address is not aligned\n", addr);
    	}
    	else{
    	    int to_write = atoi(strtok(NULL, " \n"));
    	    int fd = open(readfifo, O_WRONLY);
    	    write(fd, to_write, 4);
    	    printf("Integer %d written to address: %d\n", to_write, addr);
    	    close(fd);
    	}
    	goto prompt;
    }
    
    /* Exit */
    else if (!strcmp(command, commands[7])){
        int i;
    	for (i = 0; i < 4; i++){
    	    if (threads[i]){
    	        pthread_kill(&threads[i], SIGTERM);
    	        threads[i] = NULL;
    	        num_threads--;
    	    }
    	}
    	free(*threads);
    	free(*pageTables);
        return 0;
    }
    
    /* Inalid command */
    else{
        printf("Invalid command\n");
        goto prompt;
	}
}
