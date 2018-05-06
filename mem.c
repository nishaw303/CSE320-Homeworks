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

	while(1){
		sleep(5);
		
		/* Get request and send reply */
    	char buff[32];
    	int fd = open(readfifo, O_RDONLY);
    	read(fd, buff, 32);
    	close(fd);
    	
    	int addr = atoi(buff);
    	char reply[32];
    	sprintf(reply, "%d", memory[addr]);
    	int fd2 = open(readfifo, O_WRONLY);
    	write(fd2, reply, 32);
    	close(fd2);
    	
    	/* For writes only */
    	char buff2[32];
    	int fd3 = open(writefifo, O_RDONLY);
    	read(fd3, buff2, 32);
    	int addr2 = atoi(strtok(buff2, " \0\n"));
    	int val = atoi(strtok(NULL, " \0\n"));
    	memory[addr2] = val;
    	close(fd3);
	}
}