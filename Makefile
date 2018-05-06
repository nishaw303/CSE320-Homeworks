make: main.c mem.c
	gcc -pthread -o main main.c
	gcc -pthread -o mem mem.c
	
clean:
	rm -f main mem