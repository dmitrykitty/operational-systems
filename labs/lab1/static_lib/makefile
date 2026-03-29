CC = gcc
#CFLAGS = -Wall -Wextra -std=c11

# -L. -lsort -> search for libmysort.a or libmysort.so in current directory
program: main.c libsort.a
	$(CC) main.c -L. -lsort -o program

bubble_sort.o: bubble_sort.c sort.h
	$(CC) -c bubble_sort.c -o bubble_sort.o

selection_sort.o: selection_sort.c sort.h
	$(CC) -c selection_sort.c -o selection_sort.o

#r - replace 
#c - create if not exists
#s - create indexes
libsort.a: bubble_sort.o selection_sort.o
	ar rcs libsort.a bubble_sort.o selection_sort.o

clean:
	rm -f *.o *.a program