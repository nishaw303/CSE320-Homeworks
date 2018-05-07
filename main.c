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
pthread_t threads[4] = {0};

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


/* Cache structs */
struct cache{
    int addr;
    int val;
};


/* Actual page tables for "processes" */
struct first_level_pt* pageTables[4];


/* Cache array */
struct cache* directCache[4];

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
                if (pageTables[i]->ptr[j]->virt_addr[k] == 0){
                	if (!(!i && !j && !k)){
                    	pageTables[i]->ptr[j]->virt_addr[k] = i*256+k;
                    	return i*256+k;
                    }
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

char* cse320_phys_to_virt(int phys_addr){
	int i = phys_addr / 256;
	int temp = phys_addr % 256;
	int N = temp / 32;
	int M = temp % 32;
    char* phys_addr_str = (char*)malloc(sizeof(char) * 22);
    int q;
    for (q = 9; q > -1; q--){
        phys_addr_str[q] = (N % 2) + '0';
        N = N / 2;
        phys_addr_str[q + 11] = (M % 2) + '0';
        M = M / 2;
    }
    phys_addr_str[10] = ' ';
    phys_addr_str[21] = '\0';
    return phys_addr_str;
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
    mkfifo(readfifo, 0666);
    mkfifo(writefifo, 0666);
    
    /* Initialize the cache */
    int c;
    for (c = 0; c < 4; c++){
        directCache[c] = (struct cache*)malloc(sizeof(struct cache));
        directCache[c]->addr = -1;
    }
    
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
    	        threads[i] = 0;
    	        pthread_kill(thread_id, SIGTERM);
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
        unsigned long thread_id_temp = 0;
        for (i = 0; i < 4; i++){
            if (threads[i] == thread_id){
                thread_id_temp = thread_id;
                break;
            }
        }
        if (!thread_id_temp){
            printf("Invalid thread ID\n");
            goto prompt;
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
                        printf("Virtual memory address: 0000000000 0000000000 000000000000\n");
                    }
                    if (pageTables[i]->ptr[j]->virt_addr[k]){
                    	char* virt_addr = cse320_phys_to_virt(pageTables[i]->ptr[j]->virt_addr[k]);
                        printf("Virtual memory address: %s 000000000000\n", virt_addr);
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
        int i;
        unsigned long thread_id_temp = 0;
        for (i = 0; i < 4; i++){
            if (threads[i] == thread_id){
                thread_id_temp = thread_id;
                break;
            }
        }
        if (!thread_id_temp){
            printf("Invalid thread ID\n");
            goto prompt;
        }
        int out = cse320_malloc(thread_id);
        if (out == -1){
            printf("Exceeds max virtual memory allocation\n");
            goto prompt;
        }
        char* virt_addr = cse320_phys_to_virt(out);
        printf("Successfully allocated virtual memory at address: %s 000000000000\n", virt_addr);
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
        unsigned long thread_id_temp = 0;
        for (i = 0; i < 4; i++){
            if (threads[i] == thread_id){
                thread_id_temp = thread_id;
                break;
            }
        }
        if (!thread_id_temp){
            printf("Invalid thread ID\n");
            goto prompt;
        }
    	int addr = cse320_virt_to_phys(strtok(NULL, " \n"), i);
    	if (addr < 0){
    	    printf("Address out of range\n");
    	}
    	else if (addr % 4 != 0){
    	    printf("Address is not aligned\n");
    	}
    	else{
    	    
    	    
    	    /* In comes the cache */
    	    int d;
    	    for (d = 0; d < 4; d++){
    	        if (directCache[d]->addr == addr){
    	            printf("Cache hit: integer is %d", directCache[d]->val);
    	            goto prompt;
    	        }
    	    }
    	    printf("Cache miss: searching memory...");
    	    
    	    
    	    /* First we send a request to the memory */
    	    char temp[32];
    	    sprintf(temp, "%d", addr);
    	    int fd = open(readfifo, O_WRONLY);
    	    write(fd, temp, 32);
    	    close(fd);
    	    
    	    /* Now we wait for the reply */
            char buff[32];
    	    int fd2 = open(readfifo, O_RDONLY);
    	    read(fd2, buff, 32);
    	    char* virt_addr = cse320_phys_to_virt(addr);
    	    printf("Integer found at address %s 000000000000: %s\n", virt_addr, buff);
    	    close(fd2);
    	    
    	    /* Check if cache is full */
    	    for (d = 0; d < 4; d++){
    	        if (directCache[d]->addr == -1){
    	            directCache[d]->addr = addr;
    	            directCache[d]->val = atoi(buff);
    	            goto prompt;
    	        }
    	    }
    	    
    	    /* Cache is full, now we have to evict a cache line */
    	    int ran = rand() % 4; /* Pick a random number between 0 and 3 */
    	    directCache[ran]->addr = addr;
    	    directCache[ran]->val = atoi(buff);
    	    printf("Eviction: evicted cache line %d", ran);
    	    
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
        unsigned long thread_id_temp = 0;
        for (i = 0; i < 4; i++){
            if (threads[i] == thread_id){
                thread_id_temp = thread_id;
                break;
            }
        }
        if (!thread_id_temp){
            printf("Invalid thread ID\n");
            goto prompt;
        }
    	int addr = cse320_virt_to_phys(strtok(NULL, " \n"), i);
    	if (addr < 0){
    	    printf("Address out of range\n");
    	}
    	else if (addr % 4 != 0){
    	    printf("Address is not aligned\n");
    	}
    	else{
    	    char buff1[16];
    	    char buff2[16];
    	    char* int_out = strtok(NULL, " \n");
    	    sprintf(buff1, "%d ", addr);
    	    sprintf(buff2, "%s", int_out);
    	    char* to_write = strcat(buff1, buff2);
            int fd = open(writefifo, O_WRONLY);
            write(fd, to_write, 32);
            char* virt_addr = cse320_phys_to_virt(addr);
    	    printf("Integer %s written to address: %s 000000000000\n", int_out, virt_addr);
            close(fd);
    	    
    	    /* Check if cache is full */
    	    int d;
    	    for (d = 0; d < 4; d++){
    	        if (directCache[d]->addr == -1){
    	            directCache[d]->addr = addr;
    	            directCache[d]->val = atoi(int_out);
    	            goto prompt;
    	        }
    	    }
    	}
    	goto prompt;
    }
    
    /* Exit */
    else if (!strcmp(command, commands[7])){
        int i;
    	for (i = 0; i < 4; i++){
    	    if (threads[i]){
    	        pthread_kill(threads[i], SIGTERM);
    	        threads[i] = 0;
    	        num_threads--;
    	    }
    	}
    	free(*pageTables);
    	unlink(readfifo);
    	unlink(writefifo);
        return 0;
    }
    
    /* Inalid command */
    else{
        printf("Invalid command\n");
        goto prompt;
	}
}
