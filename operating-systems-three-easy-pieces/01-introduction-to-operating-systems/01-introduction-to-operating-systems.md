## 01. Introduction to Operating Systems

### How the CPU works
At a high level, the processor repeats the same cycle:
1. **fetch** — get instruction from memory  
2. **decode** — understand what it means  
3. **execute** — perform it  

A program is just a sequence of instructions executed by the CPU.

---

### CPU virtualization

Example:

```c
/*
To execute few processes in one command
./cpu "A" & ./cpu "B" & ./cpu "C" & ./cpu "D"
*/

int main(int argc, char* argv[]){
    if(argc < 2){
        perror("usage: cpu <string>\n");
    }

    char* str = argv[1]; 
    int n = 10; 
    while(n > 0){
        printf("%s\n", str); 
        n--;
    }
    return 0;
}
````

Running:

```bash
./cpu "A" & ./cpu "B" & ./cpu "C" & ./cpu "D"
```

starts a few separate processes.

**Important:** the output order is **not defined**.
The OS scheduler decides which process runs at a given moment, so the printed lines may be mixed in different ways.

The operating system creates the illusion that many programs run at the same time, even if there is only one CPU (or a small number of CPUs). This is called **CPU virtualization**.

---

### Memory virtualization

Example:

```c
int main(){

    // int* p = malloc(sizeof(int));
    int* p = calloc(1, sizeof(int));
    if(p == NULL){
        perror("Allocating went wrong");
        return 1; 
    }

    // getpid() -> returns process ID
    // printf("%p") expects (void*)
    printf("(%d) memory address of p:%p\n", getpid(), (void*)p);

    int n = 10; 
    while(n > 0) {
        *p = *p + 1;     
        printf("(%d) p:%d\n", getpid(), *p);
        n--;
    }

    free(p);
    return 0;
}
```

#### Notes

* `calloc()` allocates memory and sets it to `0`
* `malloc()` allocates memory but does **not** initialize it
* `%p` is used to print an address, so it is good practice to cast pointer to `(void*)`

If you run the same program multiple times, different processes may print the **same address** for `p`.

This does **not** mean they use the same physical memory.

Each process has its own private **virtual address space**, and the OS maps it to physical memory.
This is called **memory virtualization**.

---

### Concurrency

```c
volatile int counter = 0; 
int loops; 

void* worker(void* arg){
    for(int i = 0; i < loops; i++){
        counter++; 
    }
    return NULL; 
}

int main(int argc, char* argv[]){
    if(argc < 2){
        perror("usage: ./code <value>\n");
        return 1; 
    }

    loops = atoi(argv[1]);
    pthread_t p1, p2; 

    printf("Initial value: %d\n", counter);

    // pthread_create(thread, attr, start_routine, arg)
    // thread         -> pointer to variable where thread ID will be stored
    // attr           -> thread attributes (NULL = default attributes)
    // start_routine  -> function that the new thread will execute
    // arg            -> single argument passed to that function
    pthread_create(&p1, NULL, worker, NULL);
    pthread_create(&p2, NULL, worker, NULL);

    // pthread_join(thread, retval)
    // thread -> thread to wait for
    // retval -> pointer used to receive the return value of the thread function
    //           (NULL = we do not care about the returned value)
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    printf("Final value: %d\n", counter);
    return 0;
}
````

This example shows a **race condition**.

Two threads execute the same function and both increment the shared variable `counter`.

For large values, e.g. `loops = 100'000`, the final result may be incorrect, even though both threads should together increment the counter `2 * loops` times.

The reason is that:

```c
counter++;
```

is **not atomic**. It is usually performed in three steps:

1. load `counter` from memory
2. increment the value
3. store it back to memory

If two threads read the same old value at the same time, one update can overwrite the other.
As a result, some increments are lost.

---

### Persistence

To store data **persistently**, a computer uses storage devices such as:
- **hard disk drive (HDD)**
- **solid-state drive (SSD)**

The part of the operating system responsible for managing stored data is the **file system**.  
Its job is to organize files on disk and provide a simple interface for programs to create, read, write, and delete files.

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    char *file_name = "file.txt";

    // 0700 = S_IRWXU = read, write, execute for user/owner
    int file = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0700);

    if (file < 0) {
        perror("open");
        return 1;
    }

    // "Hello World\n" has 12 bytes
    int written = write(file, "Hello World\n", 12);
    if (written != 12) {
        perror("write");
        close(file);
        return 1;
    }

    close(file);
    return 0;
}
```

#### Basic file operations

To save data to disk, a program usually performs three main steps:

1. **open / create** a file
2. **write** data to it
3. **close** the file

These operations are performed through **system calls**, which transfer control from **user space** to **kernel space**.

#### Why file systems are needed

The file system does much more than just write bytes to disk. It must:

* decide where the new data will be stored,
* keep track of file metadata,
* manage internal data structures,
* issue I/O requests to the storage device.

Low-level device handling is complex, so the OS provides a simple abstraction through system calls such as `open()`, `write()`, and `close()`.

#### Performance and reliability

For performance reasons, file systems often **delay writes** and combine them into larger batches.

To protect data in case of a crash during writing, many file systems use special recovery techniques, for example:

* **journaling**
* **copy-on-write**

These mechanisms help the system recover to a consistent state after failure.
