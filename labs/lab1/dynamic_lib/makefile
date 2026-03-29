CC = gcc

program: main.o libsort.so
	$(CC) main.o -o program -ldl

main.o: main.c
	$(CC) -c main.c -o main.o

#PIC - position independent code
bubble_sort.o: bubble_sort.c sort.h
	$(CC) -c -fPIC bubble_sort.c -o bubble_sort.o

libsort.so: bubble_sort.o
	$(CC) -shared -o libsort.so bubble_sort.o

clean:
	rm -f *.o *.so program