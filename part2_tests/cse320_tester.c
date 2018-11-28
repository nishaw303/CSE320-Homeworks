#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cse320_functions.h"

void main(){
    
    int* temp = cse320_malloc(sizeof(int));
    void* temp2 = cse320_malloc(sizeof(void*));
    struct addr_in_use* temp3 = cse320_malloc(sizeof(struct addr_in_use));
    
	struct Node* node = Addr_head;
	printf("\nList of Addresses:\n");
	while (node != NULL){
	    printf("    Addr = %lu\n", (uintptr_t) ((struct addr_in_use*)node->data)->addr);
	    printf("        ref_count = %lu\n", (uintptr_t) ((struct addr_in_use*)node->data)->ref_count);
	    node = node->next;
	}
	
	cse320_free(temp);
	
	node = Addr_head;
	if (node == NULL) printf("List of Addresses:\n    No list\n");
	else{
	    printf("\nList of Addresses:\n");
	    while (node != NULL){
	        printf("    Addr = %lu\n", (uintptr_t) ((struct addr_in_use*)node->data)->addr);
	        printf("        ref_count = %lu\n", (uintptr_t) ((struct addr_in_use*)node->data)->ref_count);
	        node = node->next;
	    }
	}
	
	cse320_free(temp2);
	cse320_free(temp3);
	
	node = Addr_head;
	if (node == NULL) printf("\nList of Addresses:\n    No nodes\n");
	else{
	    printf("List of Addresses:\n");
	    while (node != NULL){
	        printf("    Addr = %lu\n", (uintptr_t) ((struct addr_in_use*)node->data)->addr);
	        printf("        ref_count = %lu\n", (uintptr_t) ((struct addr_in_use*)node->data)->ref_count);
	        node = node->next;
	    }
	}
	
	FILE* temp4 = cse320_fopen("new_file.txt", "r");
    FILE* temp5 = cse320_fopen("test.txt", "r");
    FILE* temp6 = cse320_fopen("new_file.txt", "r");
    
	node = File_head;
	printf("\nList of Files:\n");
	while (node != NULL){
	    printf("    Filename = %s\n", ((struct file_in_use*)node->data)->filename);
	    printf("        File_desc = %lu\n", (uintptr_t) ((struct file_in_use*)node->data)->file_desc);
	    printf("        ref_count = %lu\n", (uintptr_t) ((struct file_in_use*)node->data)->ref_count);
	    node = node->next;
	}
	
	cse320_fclose(temp4);
	
	node = File_head;
	if (node == NULL) printf("List of Files:\n    No nodes\n");
	else{
	    printf("\nList of Files:\n");
	    while (node != NULL){
	        printf("    Filename = %s\n", ((struct file_in_use*)node->data)->filename);
	        printf("        File_desc = %lu\n", (uintptr_t) ((struct file_in_use*)node->data)->file_desc);
	        printf("        ref_count = %lu\n", (uintptr_t) ((struct file_in_use*)node->data)->ref_count);
	        node = node->next;
	    }
	}
	
	cse320_fclose(temp5);
	cse320_fclose(temp6);
	
	node = File_head;
	if (node == NULL) printf("\nList of Files:\n    No nodes\n");
	else{
	    printf("\nList of Files:\n");
	    while (node != NULL){
	        printf("    Filename = %s\n", ((struct file_in_use*)node->data)->filename);
	        printf("        File_desc = %lu\n", (uintptr_t) ((struct file_in_use*)node->data)->file_desc);
	        printf("        ref_count = %lu\n", (uintptr_t) ((struct file_in_use*)node->data)->ref_count);
	        node = node->next;
	    }
	}
	
	cse320_free(temp);
	cse320_fclose(temp4);
}