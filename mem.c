#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <limits.h>
#include <signal.h>
#include <pthread.h>


int main(){
	
	int memory[256]; /* 256 ints of size 4 bytes = 1024 bytes total memory, all aligned */
	char* readfifo = "/tmp/readfifo"; /* Fifo to send back the integer */
	char* writefifo = "/tmp/writefifo"; /* Fifo to get requests and writes */
	mkfifo(readfifo, 0666);
	mkfifo(writefifo, 0666);

	while(1){
		sleep(5);
		
		/* Get request and send reply */
    	char buff[32];
    	buff[0] = '\0';
    	int fd = open(readfifo, O_RDONLY);
    	read(fd, buff, 32);
    	close(fd);
    	
    	if (!(buff[0] == '\0')){
    	    int addr = atoi(buff);
    	    char reply[32];
    	    sprintf(reply, "%d", memory[addr]);
    	    int fd2 = open(readfifo, O_WRONLY);
    	    write(fd2, reply, 32);
    	    printf("Integer %d was accessed at %d\n", memory[addr], addr);
    	    close(fd2);
    	}
    	
    	/* For writes only */
    	char buff2[32];
    	buff2[0] = '\0';
    	int fd3 = open(writefifo, O_RDONLY);
    	read(fd3, buff2, 32);
    	if (!(buff2[0] == '\0')){
    	    int addr2 = atoi(strtok(buff2, " \0\n"));
    	    int val = atoi(strtok(NULL, " \0\n"));
    	    memory[addr2] = val;
    	    printf("Integer %d was writter to memory at %d\n", val, addr2);
    	}
    	close(fd3);
	}
}