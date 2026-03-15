CC = gcc
#CFLAGS = -Wall -Wextra -std=c11

#-L. -lmysort - search for libmysort.a or libmysort.so inside curretn catalog
program: main.c libmysort.a
	$(CC) main.c -L. -lmysort -o program

sort.o: sort.c sort.h
	$(CC) -c sort.c -o sort.o

#r - replace 
#c - create if not exists
#s = create indexes
libmysort.a: sort.o
	ar rcs libmysort.a sort.o

clean:
	rm -f *.o *.a program