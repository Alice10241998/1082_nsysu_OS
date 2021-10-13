all: hw3_part2 dep
hw3_part2: hw3_part2.o
	g++ -o hw3_part2 hw3_part2.o -lpthread
hw3_part2.o:hw3_part2.cpp
	g++ -c hw3_part2.cpp
dep:
	gcc -MM hw3_part2.cpp > dep
clean:
	rm -f *.o dep hw3_part2
