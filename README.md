# Operating Systems — Lab Notes

This README is a personal study guide for the **Operating Systems** course.
It is meant to help me remember **what I built, how I built it, and which commands / concepts are worth memorizing** for labs, quizzes, and exams.

# General Reference

## 1. Working in terminal

### Basic navigation

```bash
pwd          # print current directory
ls           # list files
ls -la       # list files including hidden ones
cd folder    # enter folder
cd ..        # go one level up
mkdir name   # create directory
rm file      # remove file
rm -f file   # remove file without confirmation
rm -r dir    # remove directory recursively
clear        # clear terminal
```

### Useful WSL commands

```bash
explorer.exe .    # open current WSL folder in Windows Explorer
code .            # open current folder in VS Code (when WSL integration works)
which code        # check where the `code` command points
```

### Archive commands

Create a `.tar.gz` archive:

```bash
tar -czf archive_name.tar.gz folder_name
```

Flags:

* `c` — create archive
* `z` — compress with gzip
* `f` — output file name follows

Show archive contents:

```bash
tar -tzf archive_name.tar.gz
```

Extract archive:

```bash
tar -xzf archive_name.tar.gz
```

---

## 2. C compilation pipeline

In C, building a program usually happens in stages:

```text
.c source files -> .o object files -> executable
```

### Important compiler flags

#### `-c`

Compile only, do not link.

```bash
gcc -c main.c -o main.o
```

This creates an object file `main.o`.

#### `-o`

Specify output file name.

```bash
gcc main.c -o program
```

Creates an executable named `program`.

#### `-Wall`

Enable common warnings.

#### `-Wextra`

Enable additional warnings.

#### `-std=c11`

Compile using the C11 standard.

#### `-fPIC`

Generate **Position Independent Code**. Used when building object files for a **dynamic library** (`.so`).

#### `-shared`

Build a **shared library** (`.so`) instead of a normal executable.

#### `-L.`

Tell the linker to search for libraries in the current directory.

#### `-lname`

Link with library `libname.a` or `libname.so`.

Example:

```bash
gcc main.c -L. -lsort -o program
```

This means:

* search libraries in current directory,
* find `libsort.a` or `libsort.so`,
* link it into `program`.

#### `-ldl`

Link against the dynamic loader library when using:

* `dlopen`
* `dlsym`
* `dlclose`

---

## 3. Header files, source files, object files, libraries

### `.h` file

A header contains **declarations**, not implementation.

Example:

```c
void bubble_sort(float* arr, int n);
```

It tells the compiler:

> a function with this signature exists.

### `.c` file

A source file contains the **implementation**.

### `.o` file

An object file is a compiled `.c` file that is not yet a full program.

### Static library `.a`

A static library is an archive of object files.

Example:

```bash
ar rcs libsort.a bubble_sort.o selection_sort.o
```

### Dynamic library `.so`

A shared library loaded dynamically by the system, or manually with `dlopen`.

---

## 4. Makefile basics

A `Makefile` automates compilation.

### General structure

```makefile
target: dependencies
<TAB>command
```

**Important:** commands must start with a **TAB**, not spaces.

### Example

```makefile
program: main.o sort.o
	gcc main.o sort.o -o program
```

This means:

* to build `program`, `main.o` and `sort.o` are needed;
* then run the linking command.

### Typical targets

#### `all`

A conventional default target.

```makefile
all: program
```

When running:

```bash
make
```

`make` builds the first target, usually `all`.

#### `clean`

Remove generated files.

```makefile
clean:
	rm -f *.o *.a *.so program
```

### Useful commands

```bash
make         # build default target
make clean   # remove generated files
```

---

## 5. Arrays, pointers, dynamic memory

## Static array

```c
float arr[] = {64, 34, 25, 12};
```

* size known at initialization,
* allocated automatically,
* no `free()` needed.

## Dynamic array with `calloc`

```c
float* arr = calloc(n, sizeof(float));
```

* memory is allocated dynamically,
* initialized to zero,
* must be released with `free(arr);`

## Dynamic array with `malloc`

```c
float* arr = malloc(n * sizeof(float));
```

* memory allocated dynamically,
* values are uninitialized,
* must be released with `free(arr);`

### Difference between `malloc` and `calloc`

* `malloc(size)` — allocates memory, contents unspecified
* `calloc(count, size)` — allocates memory and zeros it

### Always check allocation

```c
if (arr == NULL) {
    fprintf(stderr, "Allocation failed\n");
    return 1;
}
```

---

## 6. Pointer basics

### Address-of operator `&`

```c
int x = 10;
int *p = &x;
```

* `x` — value
* `&x` — address of `x`
* `p` — pointer storing that address

### Dereference operator `*`

```c
*p = 20;
```

Means:

> go to the address stored in `p` and modify the value there.

### In declarations

```c
int *p;
```

Means:

> `p` is a pointer to `int`

### Function parameters

If you want a function to modify the original value, pass a pointer:

```c
void change(int *x) {
    *x = 100;
}
```

Call:

```c
int a = 5;
change(&a);
```

---

## 7. Function pointers

### Example declaration

```c
void (*f)(float*, int);
```

This means:

* `f` is a pointer,
* to a function,
* taking `(float*, int)`,
* returning `void`.

### With typedef

```c
typedef void (*sort_ptr)(float*, int);
```

Now `sort_ptr` is a type name for such a function pointer.

Then:

```c
sort_ptr bubble_sort;
```

is easier to read.

### Why the `*` is before the name

Because it is not a function itself, but a **pointer to a function**.

Compare:

```c
void f(float*, int);      // function
void (*f)(float*, int);   // pointer to function
```

The parentheses are essential.

---

## 8. Static libraries

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

---

## 9. Dynamic libraries

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

## 10. `dlopen`, `dlsym`, `dlclose`

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

---
# Labs workflow:
- [**lab1**](https://github.com/dmitrykitty/operating-systems/tree/main/lab1)
