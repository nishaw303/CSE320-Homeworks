#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  /*
  * Dummy values
  */
  int id = 1;
  char* first_name = "Sergey";
  char* last_name = "Madaminov";
  float gpa = 3.41;
  char* major = "CSE";
  /*
  * This formatting for the string
  * that you are expected to follow
  */
  printf("%d %s %s %.2f %s\n", id, first_name, last_name, gpa, major);
  
  int op;
  int vflag = 0, iglaf = 0, fflag = 0, mflag = 0, oflag = 0;
  int id = 0;
  char *id, *lastname, *major, *filename;
  while ((op = getop(argc, argv, "v:ifmo")) != -1){
  	switch(c){
  		case "v":
  			vflag = 1;
  			break;
  		case "i":
  			iflag = 1;
  			id = optarg;
  			break;
  		case "f":
  			fflag = 1;
  			lastname = optarg;
  			break;
  		case "m":
  			mflag = 1;
  			major = optarg;
  			break;
  		case "o":
  			oflag = 1;
  			filename = optarg;
  			break;
  	}
  }
  
  if ((optind + 1) > argc){
  	printf("NO QUERY PROVIDED");
  	return 1;
  }
  else if (vflag && (iflag || fflag || mflag)){
  	printf("OTHER ERROR");
  	return 1;
  }
  
  return 0;
}
