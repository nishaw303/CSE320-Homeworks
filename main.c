#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Function to find string length */
int strleng(char *string){
	
	int i = 0;
	while( *string != '\0'){
		string++;
		i++;
	}
	
	return i;
}

/* Function to check string equality */
int strcomp(char *string1, char *string2){

	while(*string1 != '\0' && *string1 == *string2){
		string1++;
		string2++;
	}
	if (*string1 == *string2){
		return 1;
	}
	
	return 0;
}

/* Function to iterate through string tokens, imitates existing strtok */
char* strtok(char *str, char *comp){

	static int pos;
	static char *s;	
	int i = 0, start = pos;

	if(str != NULL)
		s = str;
	
	i = 0;
	int j = 0;
	for (pos; *(s + pos) != '\0'; pos++){
		j = 0;
		for (j; *(comp + j) != '\0'; j++){
			if (*(s + pos) == *(comp + j)){
				*(s + pos) = '\0';
				pos = pos + 1;				
				if (*(s + start) != '\0')
					return (s + start);
				else{
					start = pos;
					pos--;
					break;
				}
			}
		}
	}
	*(s + pos) = '\0';
	if (*(s + start) == '\0'){
		return NULL;
	}
	else{
		return (s + start);
	}
}

/* Function to copy strings */
void strcopy(char *temp, char *tocopy){

	int i = 0;
	while(*(tocopy + i) != '\0'){
		*(temp + i) = *(tocopy + i);
		i++;
	}
	*(temp + i) = '\0';
}

/* Our student record structure */
typedef struct student_records { 
	
	int id;
	char *firstname;
	char *lastname;
	char *major;
	double gpa;
	struct student_records *next;
} student_records;

student_records* make(int id, char *firstname, char *lastname, double gpa, char *major, student_records* next){
	
	student_records* new = (student_records*)malloc(sizeof(student_records));
	new->id = id;
	new->firstname = firstname;
	new->lastname = lastname;
	new->gpa = gpa;
	new->major = major;
	
	return new;
}

student_records* prepend(int id, char *firstname, char *lastname, double gpa, char *major, student_records* head){

	student_records* new = make(id, firstname, lastname, gpa, major, head);
	head = new;
	
	return head;
}

student_records* searchid(int id, student_records* head){

 	student_records *cursor = head;
 	while(cursor != NULL){
 		if (cursor->id == id){
 			return cursor;
 		}
 		cursor = cursor->next;
 	}
 	
 	return NULL;
}

student_records* delfront(student_records* head){
	
	student_records *temp = head;
	head = head->next;
	temp->next = NULL;
	if (temp->id == head->id){
		head = NULL;
	}
	free(temp);
	
	return head;
}

student_records* delback(student_records* head){

	if (head == NULL){
		return NULL;
	}
	
	student_records *cursor = head;
	student_records *back = NULL;
	while(cursor->next != NULL){
		back = cursor;
		cursor = cursor->next;
	}
	if (back != NULL){
		back->next = NULL;
	}
	if (cursor == head){
		head == NULL;
	}
	free(cursor);
	
	return head;
}

student_records* delete(int id, student_records* head){

	student_records* temp = searchid(id, head);
	if (temp == NULL){
		return NULL;
	}

	if (head->id == temp->id){
		head = delfront(head);
		return head;
	}
	else if (temp->next == NULL){
		head = delback(head);
		return head;
	}
	
	student_records* cursor = head;
	while(cursor != NULL){
		if (cursor->next == temp){
			break;
		}
		cursor = cursor->next;
	}
	if (cursor != NULL){
		student_records* temp2 = cursor->next;
		cursor->next = temp2->next;
		temp2->next = NULL;
		free(temp2);
	}
	
	free(temp);
	return head;
}
		
/* Main function */
int main(int argc, char **argv) {

  	FILE *mainfile;
  	
	/* First check if input file exists */
	if ((mainfile = fopen(*(argv + 1), "r")) == NULL){
  		printf("FAILED TO PARSE FILE\n");
  		return 1;
  	}
  	
	extern char *optarg;
	extern int optind;
  	int op;
  	int vflag = 0, iflag = 0, fflag = 0, mflag = 0, oflag = 0, error = 0;
  	int id = 0;
  	char *lastname, *major, *filename;
  	FILE *outputfile;
  
  	/* Taking in arguments from command line */
  	while ((op = getopt(argc, argv, "vi:f:m:o:")) != -1){
  		switch(op){
  			case 'v':
  				vflag = 1;
  				break;
  			case 'i':
  				iflag = 1;
  				id = atoi(optarg);
  				break;
  			case 'f':
  				fflag = 1;
  				lastname = optarg;
  				break;
  			case 'm':
  				mflag = 1;
  				major = optarg;
  				break;
  			case 'o':
  				oflag = 1;
  				filename = optarg;
  				break;
  			case '?':
  				error = 1;
  				break;
  		
  		}
  	}
  	
  	/* Checking if there is at least on argument, opening source file and error checking */
  	if (argc < 3){
  		printf("NO QUERY PROVIDED\n");
  		return 1;
  	}
  	if ((vflag && (iflag || fflag || mflag)) || error){
  		printf("OTHER ERROR\n");
  		return 1;
  	}
  	if (oflag && ((fopen(filename, "r")) != NULL)){
  		printf("FILE EXISTS\n");
  		return 1;
  	}
  	
  	/* Create linked list */
  	student_records* head;
  	
  	
  	/* Reading from input file */
  	char line[256];
  	char *token = NULL;
  	char *delm = " ";
  	size_t i = 0;
  	ssize_t read;
  	while(fgets(line, sizeof(line), mainfile) != NULL){

		char *temp = malloc(strleng(line));
		strcopy(temp, line);
  		token = strtok(temp, delm);
  		
  		if (strcomp(token, "ADD")){
  			int id = atoi(strtok(temp, delm));
  			char *firstname = strtok(temp, delm);
  			char *lastname = strtok(temp, delm);
  			double gpa = atof(strtok(temp, delm));
  			char *major = strtok(temp, delm);
  			
  			head = make(id, firstname, lastname, gpa, major, NULL);
  			printf("%d %s %s %.2f %s\n", id, firstname, lastname, gpa, major);
  		}
  		else if (strcomp(token, "UPDATE")){
  		
  		}
  		else if (strcomp(token, "DELETE")){
  		
  		}
  		else{
  		
  		}
  		
  	}
	
	fclose(mainfile);
  	return 0;
}