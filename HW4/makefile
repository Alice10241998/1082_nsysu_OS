CC = gcc -Wall -std=c99
t1 = main
all:
	$(CC) -c mm.c
	$(CC) -c $(t1).c
	$(CC) -o $(t1) $(t1).o mm.o
clean:
	rm -f $(t1)
	rm -f *.o
	
