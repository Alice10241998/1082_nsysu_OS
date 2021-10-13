all: hw1_part2

shell: hw1_part2.o
	gcc -Wall -o hw1_part2 hw1_part2.o

shell.o:
	gcc -Wall -c hw1_part2.c

dep:
	gcc -M *.c > .depend

clean:
	rm -f hw1_part2 *.o *~ *.*~
