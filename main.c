#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

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

student_records* fromline(char *line, student_records* head){

    char *str = line;
    char *characters = "0123456789.";
    char newString[5][10]; 
    int i = 0, j = 0, ctr = 0;
    
    for(i = 0; i <= (strleng(str)); i++){
    	if(j > 10){	//Means one of the paramaters was larger than 10
    		return NULL;
    	}
        if(*(str + i) == ' ' || *(str + i) == '\0'){
            *((char *)newString + ctr * 10 + j) = '\0';
            ctr++;
            j = 0;
        }
        else{
            *((char *)newString + ctr * 10 + j) = *(str + i);
            j++;
        }
    }
    /* Checking all formatting for inputs */
    char *temp = (char *)malloc(sizeof(*(newString + 1)));
    strcopy(temp, *(newString + 1));
    while(*temp){
    	if (isdigit(*temp) == 0){
    		return NULL;
    	}
    	temp++;
    }
    
    char *temp4 = (char *)malloc(sizeof(*(newString + 4)));
    strcopy(temp4, *(newString + 4));
    while(*temp4){
    	if (isdigit(*temp4) == 0){
    		if (*temp4 != '.'){
    			return NULL;
    		}
    	}
    	temp4++;
    }
    
    /* Copy all of the string fields and fix any casing errors*/
    char *firstname = (char *)malloc(sizeof(*(newString + 2)));
    strcopy(firstname, *(newString + 2));
    for (i = 0; i < strleng(firstname); i++){
    	*(firstname + i) = tolower(*(firstname + i));
    }
    *firstname = toupper(*firstname);
    
    char *lastname = (char *)malloc(sizeof(*(newString + 3)));
    strcopy(lastname, *(newString + 3));
    for (i = 0; i < strleng(lastname); i++){
    	*(lastname + i) = tolower(*(lastname + i));
    }
    *lastname = toupper(*lastname);
    
    char *major = (char *)malloc(sizeof(*(newString + 5)));
    strcopy(major, *(newString + 5));
    for (i = 0; i < strleng(major);	i++){
    	*(major + i) = toupper(*(major + i));
    }
    
    student_records* new = make(atoi(*(newString + 1)), firstname, lastname, atof(*(newString + 4)), major, NULL);
    return new;
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
  				for (int i = 0; i < strleng(optarg); i++){
  					if (isdigit(*(optarg + i)) == 0){
  						printf("OTHER ERROR\n");
  						return 1;
  					}
  				}
  				break;
  			case 'f':
  				fflag = 1;
  				lastname = optarg;
  				for (int i = 0; i < strleng(optarg); i++){
  					if (isalpha(*(optarg + i)) == 0){
  						printf("OTHER ERROR\n");
  						return 1;
  					}
  				}
  				break;
  			case 'm':
  				mflag = 1;
  				major = optarg;
  				for (int i = 0; i < strleng(optarg); i++){
  					if (isalpha(*(optarg + i)) == 0){
  						printf("OTHER ERROR\n");
  						return 1;
  					}
  				}
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
  	
  	/* Create linked list */
  	student_records* head = NULL;
  	
  	
  	/* Reading from input file */
  	char line[256];
  	char *token = NULL;
  	char *delm = ",!? \n";
  	size_t i = 0;
  	ssize_t read;
  	while(fgets(line, sizeof(line), mainfile) != NULL){

		*(line + strleng(line) - 1) = '\0';
		
		if (*line == 'A'){
  			student_records* new = fromline(line, head);
  			if (new == NULL){
  				printf("FAILED TO PARSE FILE\n");
  				return 1;
  			}
  			if (searchid(new->id, head) != NULL){
  				printf("ID NOT UNIQUE\n");
  				return 1;
  			}
  			else{		
  				if (head == NULL){
  					head = new;
  				}
  				else{
  					student_records* cursor = head;
  					while(cursor->next != NULL){
  						cursor = cursor->next;
  					}
  					cursor->next = new;
  				}
  			}
  		}
  		else if (*line == 'U'){
  			student_records* new = fromline(line, head);
  			if (new == NULL){
  				printf("FAILED TO PARSE FILE\n");
  				return 1;
  			}
  			student_records* cursor = searchid(new->id, head);
  			if (cursor == NULL){
  				printf("STUDENT RECORD CANNOT BE DELETED NOR UPDATED\n");
  				return 1;
  			}
  			else{
  				cursor->firstname = new->firstname;
  				cursor->lastname = new->lastname;
  				cursor->gpa = new->gpa;
  				cursor->major = new->major;
  			}
  		
  		}
  		else if (*line == 'D'){
  			student_records* new = fromline(line, head);
  			if (searchid(new->id, head) == NULL){
  				printf("STUDENT RECORD CANNOT BE DELETED NOR UPDATED\n");
  				return 1;
  			}
  			else{
  				delete(new->id, head);
  			}
  		}
  		else{
  			printf("FAILED TO PARSE FILE\n");
  			return 1;
  		}
  	}
	fclose(mainfile);
  	
  	/* Now that we have our linked list, we can work with the flags */
  	if (oflag && ((fopen(filename, "r")) != NULL)){
  		char *answer;
  		printf("File already exists, overwrite?: ");
  		scanf("%s", answer);
  		if (*answer == 'y'){
  			remove(filename);
  			outputfile = fopen(filename, "w");
  			student_records* cursor = head;
  			while(cursor != NULL){
  				fprintf(outputfile, "%d %s %s %.2f %s\n", cursor->id, cursor->firstname, cursor->lastname, cursor->gpa, cursor->major);
  				cursor = cursor->next;
  			}
  		}
  		else{
  			printf("FILE EXISTS\n");
  			return 1;
  		}
  	}
  	else if (oflag){
  		outputfile = fopen(filename, "w");
		student_records* cursor = head;
		while(cursor != NULL){
			fprintf(outputfile, "%d %s %s %.2f %s\n", cursor->id, cursor->firstname, cursor->lastname, cursor->gpa, cursor->major);
			cursor = cursor->next;
		}
  	}
  	if (vflag){
  		student_records* cursor = head;
  		while(cursor != NULL){
  			printf("%d %s %s %.2f %s\n", cursor->id, cursor->firstname, cursor->lastname, cursor->gpa, cursor->major);
  			cursor = cursor->next;
  		}
  	}
  	if (iflag){
  		student_records* cursor = searchid(id, head);
  		if (cursor == NULL){
  			printf("STUDENT RECORD NOT FOUND\n");
  			return 1;
  		}
  		else{
  			printf("%d %s %s %.2f %s\n", cursor->id, cursor->firstname, cursor->lastname, cursor->gpa, cursor->major);
  		}
  	}
  	if (fflag){
  		student_records* cursor = head;
  		while(cursor != NULL){
  			if (strcomp(cursor->lastname, lastname)){
  				printf("%d %s %s %.2f %s\n", cursor->id, cursor->firstname, cursor->lastname, cursor->gpa, cursor->major);
  			}
  			cursor = cursor->next;
  		}
  	}
  	if (mflag){
  		student_records* cursor = head;
  		while(cursor != NULL){
  			if (strcomp(cursor->major, major)){
  				printf("%d %s %s %.2f %s\n", cursor->id, cursor->firstname, cursor->lastname, cursor->gpa, cursor->major);
  			}
  			cursor = cursor->next;
  		}
  	}
	
  	return 0;
}