#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int trace(int r1, int c1, int A[r1][c1], int r2, int c2, int B[r2][c2]){

	int i, j, k, sum = 0;
	int C[r1][c2];
	
	for (i = 0; i < r1; i++){
		for (j = 0; j < c2; j++){
			C[i][j] = 0;
			for (k = 0; k < r2; k++){
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	
	for (i = 0; i < r1; i++){
		sum = sum + C[i][i];
	}
	
	return sum;
}

int main(int argc, char** argv){

	FILE *file = fopen(argv[1], "r");
	char buff[255];
	int r1, r2, c1, c2, i, j;
	
	fscanf(file, "%d", &r1);
	fscanf(file, "%d", &c1);
	int A[r1][c1];
	
	for (i = 0; i < r1; i++){
		for (j = 0; j < c1; j++){
			fscanf(file, "%d", &A[i][j]);
		}
	}
	
	fscanf(file, "%d", &r2);
	fscanf(file, "%d", &c2);
	int B[r2][c2];
	
	for (i = 0; i < r2; i++){
		for (j = 0; j < c2; j++){
			fscanf(file, "%d", &B[i][j]);
		}
	}
	
	int traced = trace(r1, c1, A, r2, c2, B);
	FILE* outfile;
	outfile = fopen("part2_trace.dat", "w");
	fprintf(outfile, "%d", traced);
	fclose(outfile);
	fclose(file);
	return 0;
}