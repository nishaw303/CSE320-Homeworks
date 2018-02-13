#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Function to check string length */
int stringLen(char *string){

	int len = 0;
	while(*string != '\0'){
		len++;
		string++;
	}
	return len;
}

/* Function to check string equality */
int stringComp(char *string1, char *string2){
	
	while(*string1 == *string2){
		if (*string1 == '\0' && *string2 == '\0'){
			return 1;
		}
	}
	return 0;
}

/* Our student structure */
struct Student { 
	int id;
	char *firstname;
	char *lastname;
	char *major;
	int gpa;
};

/* Main function */
int main(int argc, char **argv) {

	extern char *optarg;
	extern int optind;
  	int op;
  	int vflag = 0, iflag = 0, fflag = 0, mflag = 0, oflag = 0, error = 0;
  	int id = 0;
  	char *lastname, *major, *filename;
  	FILE *mainfile;
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
  	if (argc >= 3){
  		mainfile = fopen(*(argv + 1), "r");
  	}
  	else {
  		printf("NO QUERY PROVIDED");
  		return 1;
  	}
  	if ((vflag && (iflag || fflag || mflag)) || error){
  		printf("OTHER ERROR");
  		return 1;
  	}
  	if (oflag && outputfile = fopen(filename, "r")){
  		printf("FILE EXISTS");
  		return 1;
  	}
  	
  	

  	return 0;
}
