# Lab 1 — Libraries and Makefiles

## Topics covered

* static libraries (`.a`)
* dynamic libraries (`.so`)
* `Makefile`
* object files (`.o`)
* function pointers
* `dlopen`, `dlsym`, `dlclose`
* dynamic arrays with `calloc` and `free`

---

## 1. Static library task

### How to build manually

Compile source to object file:

```bash
gcc -c sort.c -o sort.o
```

Create static library:

```bash
ar rcs libsort.a sort.o
```

Flags for `ar`:

* `r` — replace / add files
* `c` — create archive if needed
* `s` — create symbol index

Link executable with static library:

```bash
gcc main.c -L. -lsort -o program
```

or directly:

```bash
gcc main.c libsort.a -o program
```

### Notes

* Header `.h` is still needed for declarations.
* The library contains compiled implementation.
* Static libraries are more useful when combining many `.o` files into one module.

### Goal

Create a sorting library and a `Makefile` that compiles the project.

### Important ideas

* A static library is built from object files.
* It groups multiple `.o` files into a single reusable module.
* Header file provides declarations.
* Source files provide implementation.

### Typical flow

```bash
gcc -c bubble_sort.c -o bubble_sort.o
gcc -c selection_sort.c -o selection_sort.o
ar rcs libsort.a bubble_sort.o selection_sort.o
gcc main.c -L. -lsort -o program
```

### What to remember

* `.h` contains declarations only.
* `.a` contains compiled code.
* `#include "sort.h"` is correct.
* `#include "libsort.a"` is wrong.
* You can link many `.o` files directly, but a static library helps group them into one logical module.

### Common mistakes

* forgetting function declarations in the header,
* including `.c` or `.a` instead of `.h`,
* missing `-c` when building `.o`,
* using spaces instead of TAB in `Makefile` commands,
* committing build artifacts (`*.o`, `*.a`, `program`) to Git.

---

## 2. Dynamic library task

### How to build manually

Compile source for shared library:

```bash
gcc -c -fPIC bubble_sort.c -o bubble_sort.o
```

Create shared library:

```bash
gcc -shared -o libsort.so bubble_sort.o
```

### Two ways to use a dynamic library

#### A. Normal dynamic linking

```bash
gcc main.c -L. -lsort -o program
```

The program is linked against the dynamic library during build.

#### B. Runtime loading with `dlopen`

```c
void *handle = dlopen("./libsort.so", RTLD_LAZY);
```

Then use:

* `dlsym` to get a function address,
* `dlclose` to close the library.

If using `dlopen`, compile the program with:

```bash
gcc main.c -o program -ldl
```

### Runtime loader path

Sometimes the system cannot find `.so` in the current directory.

Use:

```bash
LD_LIBRARY_PATH=. ./program
```

or:

```bash
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
./program
```

---

## `dlopen`, `dlsym`, `dlclose`

### `dlopen`

Loads a shared library during program execution.

```c
void *handle = dlopen("./libsort.so", RTLD_LAZY);
```

### `dlsym`

Gets the address of a symbol (for example, a function) from the library.

```c
sort_ptr bubble_sort = (sort_ptr)dlsym(handle, "bubble_sort");
```

### `dlclose`

Closes the dynamically loaded library.

```c
dlclose(handle);
```

### Typical flow

1. Load library with `dlopen`
2. Get function address with `dlsym`
3. Call function through function pointer
4. Close library with `dlclose`

### Goal

Modify the previous solution so that:

* the library is loaded dynamically during program execution,
* the array used in the program is dynamic (`calloc`, `free`),
* a `Makefile` is provided.

### Important ideas

* Shared library is built as `.so`.
* `-fPIC` should be used for object files that become part of the shared library.
* Program can use the library in two ways:

  1. normal dynamic linking with `-lsort`,
  2. runtime loading with `dlopen` / `dlsym` / `dlclose`.

For this lab, runtime loading is especially important if the task says the library should be loaded **during program execution**.

### Runtime loading flow

```c
void *handle = dlopen("./libsort.so", RTLD_LAZY);
sort_ptr bubble_sort = (sort_ptr)dlsym(handle, "bubble_sort");
bubble_sort(arr, n);
dlclose(handle);
```

### Dynamic array flow

```c
float* arr = calloc(n, sizeof(float));
...
free(arr);
```

### What to remember

* `calloc` allocates memory and zeroes it.
* `free` must be called for dynamically allocated memory.
* `dlsym` returns an address that must be treated as the correct function pointer type.
* Function pointer declaration must match the real function signature.

### Common mistakes

* forgetting to check if `calloc` returned `NULL`,
* forgetting to free memory,
* forgetting `-ldl` when using `dlopen`,
* forgetting `-fPIC` when building `.so`,
* not setting `LD_LIBRARY_PATH` when running the program,
* wrong braces causing `dlclose` / `free` to happen inside a loop.

---

## 3. Example Makefile patterns worth remembering

### Static library

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

program: main.o libsort.a
	$(CC) main.o -L. -lsort -o program

main.o: main.c sort.h
	$(CC) $(CFLAGS) -c main.c -o main.o

bubble_sort.o: bubble_sort.c sort.h
	$(CC) $(CFLAGS) -c bubble_sort.c -o bubble_sort.o

selection_sort.o: selection_sort.c sort.h
	$(CC) $(CFLAGS) -c selection_sort.c -o selection_sort.o

libsort.a: bubble_sort.o selection_sort.o
	ar rcs libsort.a bubble_sort.o selection_sort.o

clean:
	rm -f *.o *.a program
```

### Dynamic library with `dlopen`

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -fPIC

program: main.o libsort.so
	$(CC) main.o -o program -ldl

main.o: main.c
	$(CC) -Wall -Wextra -std=c11 -c main.c -o main.o

bubble_sort.o: bubble_sort.c sort.h
	$(CC) $(CFLAGS) -c bubble_sort.c -o bubble_sort.o

libsort.so: bubble_sort.o
	$(CC) -shared -o libsort.so bubble_sort.o

clean:
	rm -f *.o *.so program
```
