#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <libgen.h>


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

struct Node* Addr_head = NULL;
struct Node* Freed_head = NULL;
int num_addr = 0;
struct Node* File_head = NULL;
struct Node* Closed_head = NULL;
int num_file = 0;

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


/* Linked list implementation */
struct Node{
	void* data;
	struct Node* next;
};

void insert_node(struct Node** head, void* data){
	struct Node* node = (struct Node*)malloc(sizeof(struct Node));
	node->next = *head;
	node->data = data;
    *head = node;
}

int remove_node(struct Node** head, struct Node** rmv){
    struct Node* ptr = *head, *prev;
    if (ptr != NULL && ptr == *rmv){
        *head = (*rmv)->next;
        free(ptr);
        return 0;
    }
    while (ptr != NULL && ptr != *rmv){
        prev = ptr;
        ptr = ptr->next;
    }
    if (ptr == NULL) return -1;
    
    prev->next = ptr->next;
    free(ptr);
    return 0;
}


/* cse320 methods */
void* cse320_malloc(size_t size){
	pthread_mutex_lock(&lock);
	
	void* pointer;
	pointer = malloc(size);
	
	struct addr_in_use* new = (struct addr_in_use*)malloc(sizeof(struct addr_in_use));
	
	/* Return error if either malloc call returns null */
	if (!pointer || !new){
		printf("Not enough memory\n");
		errno = ENOMEM;
		pthread_mutex_unlock(&lock);
		return -1;
	}
	
	struct Node* node = Freed_head;
	while (node != NULL){
		if (node->data == (int)pointer){
			remove_node(&Freed_head, &node);
		}
		node = node->next;
	}
	
	new->addr = pointer;
	new->ref_count = 1;
	num_addr++;
	insert_node(&Addr_head, new);
	
	pthread_mutex_unlock(&lock);
	return pointer;
}

int cse320_free(void* ptr){
	pthread_mutex_lock(&lock);
	
	struct Node* node = Freed_head;
	while (node != NULL){
		if (node->data == (int)ptr){
			printf("Free: Double free attempt\n");
			errno = EADDRNOTAVAIL;
			pthread_mutex_unlock(&lock);
			return -1;
		}
		node = node->next;
	}
	
	node = Addr_head;
	while (node != NULL){
		if (((struct addr_in_use*)node->data)->addr && ((struct addr_in_use*)node->data)->addr == ptr){
			free(ptr);
			insert_node(&Freed_head, (int)ptr);
			remove_node(&Addr_head, &node);
			num_addr--;
			pthread_mutex_unlock(&lock);
			return 0;
		}
		node = node->next;
	}
	
	printf("Free: Illegal address\n");
	errno = EFAULT;
	pthread_mutex_unlock(&lock);
	return -1;
}

FILE* cse320_fopen(char* filename, const char* mode){
	pthread_mutex_lock(&lock);
	
	FILE* file;
	
	struct file_in_use* new = (struct file_in_use*)malloc(sizeof(struct file_in_use));
	
	/* Return error if malloc call returns null */
	if (!new){
		printf("Too many open files\n");
		errno = ENFILE;
		pthread_mutex_unlock(&lock);
		return -1;
	}
	
	struct Node* node = File_head;
	while (node != NULL){
		if (((struct file_in_use*)node->data)->filename == basename(filename)){
			((struct file_in_use*)node->data)->ref_count++;
			pthread_mutex_unlock(&lock);
			return ((struct file_in_use*)node->data)->file_desc;
		}
		node = node->next;
	}
	
	if (access(filename, F_OK) != -1){
		file = fopen(filename, mode);
	}
	else{
		errno = ENFILE;
		printf("Open: file not found\n");
		pthread_mutex_unlock(&lock);
		return -1;
	}
	
	node = Closed_head;
	while (node != NULL){
		if (node->data == (int)file){
			remove_node(&Freed_head, &node);
		}
		node = node->next;
	}
	
	new->filename = basename(filename);
	new->file_desc = file;
	new->ref_count = 1;
	num_file++;
	insert_node(&File_head, new);
	
	pthread_mutex_unlock(&lock);
	return file;
}

int cse320_fclose(FILE* stream){
	pthread_mutex_lock(&lock);
	
	struct Node* node = Closed_head;
	while (node != NULL){
		if (node->data == (int)stream){
			printf("Close: Ref count zero\n");
			errno = EINVAL;
			pthread_mutex_unlock(&lock);
			return -1;
		}
		node = node->next;
	}
	
	node = File_head;
	while (node != NULL){
		if (((struct file_in_use*)node->data)->filename && ((struct file_in_use*)node->data)->file_desc == stream){
			((struct file_in_use*)node->data)->ref_count--;
			if (!((struct file_in_use*)node->data)->ref_count){
				fclose(stream);
			    insert_node(&Closed_head, (int)stream);
			    remove_node(&File_head, &node);
				num_file--;
			}
			pthread_mutex_unlock(&lock);
			return 0;
		}
		node = node->next;
	}
	
	printf("Close: Illegal filename\n");
	errno = ENOENT;
	pthread_mutex_unlock(&lock);
	return -1;
}

int cse320_clean(){
	pthread_mutex_lock(&lock);
	
	struct Node* node = Addr_head;
	struct Node* next = NULL;
 
   while (node != NULL){
        next = node->next;
	    insert_node(&Freed_head, (int)((struct addr_in_use*)node->data)->addr);
		free(((struct addr_in_use*)node->data)->addr);
		free(node);
		node = next;
	}
	Addr_head = NULL;
	num_addr = 0;
	
	
	node = File_head;
	next = NULL;
	while (node != NULL){
        next = node->next;
	    insert_node(&Closed_head, (int)((struct file_in_use*)node->data)->file_desc);
		free(((struct file_in_use*)node->data)->file_desc);
		free(node);
		node = next;
	}
	File_head = NULL;
	num_file = 0;
	
	pthread_mutex_unlock(&lock);
	return 0;
}

void cse320_handler(){
	pthread_mutex_lock(&lock);

	if (pid){
		kill(pid, SIGKILL);
		pid = 0;
	}
	
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