#ifndef LIBS
#define LIBS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>

#endif

void* cse320_init(char* filename);
void* cse320_tmp_buffer_init();
void* cse320_sbrk(ssize_t size);
int cse320_check();
void cse320_free();

// TODO: add superfree EC
// TODO: add gap/canary EC
