#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

double func(int n, double c[n + 1], double x){
	double testsum = 0;
	int i;
	
	for (i = 1; i <= n; i++){
		testsum += c[i] * pow(x, i); 
	}
	testsum += c[0];
	return testsum;
}

double solver(int n, double e, double a, double b, double c[n + 1]){

	double testsum, x;
	
	while ((b - a) >= e){
		x = (a + b) / 2;
		
		if(func(n, c, x) == 0.0){
			break;
		}
		else if(func(n, c, x) * func(n, c, a) < 0){
			b = x;
		}
		else{
			a = x;
		}
	}
	return x;
}

int main(int argc, char** argv){

	int n, i;
	double e, A, B;
	FILE *file = fopen(argv[1], "r");
	char buff[255];
	
	fscanf(file, "%d", &n);
	fscanf(file, "%lf", &e);
	fscanf(file, "%lf", &A);
	fscanf(file, "%lf", &B);
	
	double c[n + 1];
	
	for (i = n; i >= 0; i--){
		fscanf(file, "%lf", &c[i]);
	}
	
	double solved = solver(n, e, A, B, c);
	FILE* outfile;
	outfile = fopen("part2_solver.dat", "w");
	fprintf(outfile, "%lf", solved);
	fclose(outfile);
	fclose(file);
	return 0;
}