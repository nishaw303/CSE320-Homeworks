part2: part2src/part2.c part2src/fib.c part2src/solver.c part2src/trace.c
	gcc -o part2 part2src/part2.c
	gcc -o fib part2src/fib.c
	gcc -o solver part2src/solver.c -lm
	gcc -o trace part2src/trace.c
	
clean:
	rm -f part2 fib solver trace