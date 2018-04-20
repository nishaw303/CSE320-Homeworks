#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>


/* Timer time (in seconds) */
int timer = 5; /* Default 5 second timer */
pid_t pid; /* Used for storing pid to be passed to handler */

/* Structs, arrays, and mutex for methods */
struct addr_in_use{
	void* addr;
	int ref_count;
};

struct file_in_use{
	char* filename;
	FILE* file_desc;
	int ref_count;
};

struct addr_in_use addr_arr[25];
int num_addr = 0;
struct file_in_use file_arr[25];
int num_file = 0;

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


/* Linked list implementation */
struct Node{
	void* data;
	struct Node* next;
};

void insert(struct Node** head, void* data){
	struct Node* node = (struct Node*)malloc(sizeof(struct Node));
	node->data = malloc(sizeof(data));
	node->next = *head;
	
	int i;
	for (i = 0; i < sizeof(data); i++)
        *(char *)(node->data + i) = *(char *)(data + i);
    *head = node;
}


/* cse320 methods */
void* cse320_malloc(size_t size){
	pthread_mutex_lock(&lock);
	
	if (num_addr > 25){
		printf("Not enough memory\n");
		errno = ENOMEM;
		pthread_mutex_unlock(&lock);
		return -1;
	}
	
	void* pointer;
	pointer = malloc(size);
	
	int i;
	for (i = 0; i < 25; i++){
		if (!addr_arr[i].addr){
			addr_arr[i].addr = pointer;
			addr_arr[i].ref_count++;
			num_addr++;
			break;
		}
	}
	
	pthread_mutex_unlock(&lock);
	return pointer;
}

int cse320_free(void* ptr){
	pthread_mutex_lock(&lock);
	
	int i;
	for(i = 0; i < 25; i++){
		if ((!addr_arr[i].addr) && (addr_arr[i].addr == ptr)){
			if (addr_arr[i].ref_count){
				addr_arr[i].ref_count--;
				free(ptr);
				pthread_mutex_unlock(&lock);
				return 0;
			}
			else{
				printf("Free: Double free attempt\n");
				errno = EADDRNOTAVAIL;
				pthread_mutex_unlock(&lock);
				return -1;
			}
		}
	}
	
	printf("Free: Illegal address\n");
	errno = EFAULT;
	pthread_mutex_unlock(&lock);
	return -1;
}

FILE* cse320_fopen(const char* filename, const char* mode){
	pthread_mutex_lock(&lock);
	
	FILE* file;
	
	if (num_file > 25){
		printf("Too many open files\n");
		errno = ENFILE;
		pthread_mutex_unlock(&lock);
		return -1;
	}
	
	int i;
	for (i = 0; i < 25; i++){
		if (file_arr[i].filename == basename(filename)){
			if (!file_arr[i].file_desc){
				file_arr[i].file_desc = fopen(filename, mode);
			}
			file_arr[i].ref_count++;
			pthread_mutex_unlock(&lock);
			return file_arr[i].file_desc;
		}
	}
	for (i = 0; i < 25; i++){
		if (!file_arr[i].filename){
			file = fopen(filename, mode);
			if (!file){
				errno = ENFILE;
				printf("Fopen error");
				pthread_mutex_unlock(&lock);
				return NULL;
			}
			file_arr[i].filename = basename(filename);
			file_arr[i].file_desc = file;
			file_arr[i].ref_count++;
			num_file++;
			break;
		}
	}
	
	pthread_mutex_unlock(&lock);
	return file;
}

int cse320_fclose(FILE* stream){
	pthread_mutex_lock(&lock);
	
	int i;
	for(i = 0; i < 25; i++){
		if ((!file_arr[i].filename) && (file_arr[i].file_desc == stream)){
			if (file_arr[i].ref_count){
				file_arr[i].ref_count--;
				if (!file_arr[i].ref_count){
					fclose(stream);
					file_arr[i].file_desc = NULL;
				}
				pthread_mutex_unlock(&lock);
				return 0;
			}
			else{
				printf("Close: Ref count zero\n");
				errno = EINVAL;
				pthread_mutex_unlock(&lock);
				return -1;
			}
		}
	}
	
	printf("Close: Illegal filename\n");
	errno = ENOENT;
	pthread_mutex_unlock(&lock);
	return -1;
}

int cse320_clean(){
	pthread_mutex_lock(&lock);
	
	int i;
	for (i = 0; i < 25; i++){
		if (addr_arr[i].ref_count > 0){
			free(addr_arr[i].addr);
			addr_arr[i].ref_count = 0;
		}
		if (file_arr[i].ref_count > 0){
			fclose(file_arr[i].file_desc);
			file_arr[i].ref_count = 0;
		}
	}
	
	pthread_mutex_unlock(&lock);
	return 0;
}

void cse320_handler(){
	pthread_mutex_lock(&lock);

	if (pid){
		kill(pid, SIGKILL);
		pid = 0;
	}
	signal(SIGALRM, cse320_handler);
	alarm(timer);
	
	pthread_mutex_unlock(&lock);
}

pid_t cse320_fork(){
	pthread_mutex_lock(&lock);
	
	if (pid = fork()){
		signal(SIGALRM, cse320_handler);
		alarm(timer);
	}
	
	pthread_mutex_unlock(&lock);
	return pid;
}

int cse320_settimer(int t){
	pthread_mutex_lock(&lock);
	
	timer = t;
	
	pthread_mutex_unlock(&lock);
	return 0;
}