#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint64_t fib(int n){

	uint64_t f[n + 1];
	uint64_t i;
	
	f[0] = 0;
	f[1] = 1;
	
	if (n < 2){
		return n;
	}
	for (i = 2; i <= n; i++){
		f[i] = f[i - 1] + f[i - 2];
	}
	
	return f[n];
}

int main(int argc, char** argv){

	int val;
	uint64_t result;
	FILE *file = fopen(argv[1], "r");
	fscanf(file, "%d", &val);
	result = fib(val);
	
	FILE* outfile;
	outfile = fopen("part2_fib.dat", "w");
	fprintf(outfile, "%lu", result);
	fclose(outfile);
	fclose(file);
	return 0;
}