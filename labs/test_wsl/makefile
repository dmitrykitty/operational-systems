CC = gcc
CFLAGS = -Wall -Wextra -std=c11

program: main.o sort.o 
	$(CC) $(CFLAGS) main.o sort.o -o program

main.o: main.c sort.h
	$(CC) $(CFLAGS) -c main.c

#-c - only compile without linking
#-o to file with name _name_
sort.o: sort.c sort.h
	$(CC) $(CFLAGS) -c sort.c

clean:
	rm -f *.o program