make: main.c mem.c
	gcc -o main main.c
	gcc -o mem mem.c
	
clean:
	rm -f main mem