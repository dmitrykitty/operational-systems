## Process API

### `fork()`
`fork()` is used to create a new process.

After calling `fork()`, there are two processes:
- **parent**
- **child**

The child is almost an exact copy of the parent, but it has its own:
- address space
- registers
- program counter
- stack


```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    printf("hello world from process(%d)\n", getpid());

    // fork() creates a new process
    int rc = fork();

    if (rc < 0) {
        perror("creating new process failed");
        exit(1);
    } else if (rc == 0) {
        // rc == 0 means: this code is running in the child
        printf("hello from child process(%d)\n", getpid());
    } else {
        // rc > 0 means: this code is running in the parent
        // rc is the PID of the child
        printf("hello from parent process(%d). I'm parent of process(%d)\n", getpid(), rc);
    }

    return 0;
}
````

### Important

The output is **not deterministic**.
After `fork()`, parent and child run independently, and the scheduler decides which one prints first.

### What does `fork()` return?

* `< 0` → error
* `0` → child process
* `> 0` → parent process, and the value is the child's PID

### Why compare with `0` if PID is some positive number?

Because the return value of `fork()` is **different in parent and child**:

* in the **child**, `fork()` returns `0`
* in the **parent**, `fork()` returns the child's PID

So `0` does **not** mean the child's real PID.
It is just a special return value that tells the process: *you are the child*.

### `exit()` vs `_exit()`

Both terminate the process, but there is a difference:

* `exit()`

  * performs normal C library cleanup
  * flushes buffered output
  * runs `atexit()` handlers

* `_exit()`

  * terminates immediately
  * does not flush stdio buffers
  * does not run `atexit()` handlers

After `fork()`, `_exit()` is often safer in the child, especially if the child will not continue normal program flow.

---

### `wait()`

`wait()` is used by the parent to wait until one of its child processes finishes.

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    printf("hello world from process(%d)\n", getpid());

    int rc = fork();

    if (rc < 0) {
        perror("creating new process failed");
        _exit(1);
    } else if (rc == 0) {
        printf("hello from child process(%d)\n", getpid());
    } else {
        int wc = wait(NULL);
        printf("hello from parent process(%d). I'm parent of process(%d), wc:%d\n",
               getpid(), rc, wc);
    }

    return 0;
}
```

Now the output becomes more predictable:

* the parent waits
* the child finishes first
* only then the parent continues

### What parameter does `wait()` take?

```c
wait(int *status)
```

It takes a pointer to an integer where the child's exit status can be stored.

### Why `NULL`?

If we write:

```c
wait(NULL);
```

it means:

* wait for a child to finish
* but do **not care** about its exit status

If you want the status:

```c
int status;
wait(&status);
```

### What does `wait()` return?

It returns the PID of the finished child process.

---

### `waitpid()`

`waitpid()` is a more flexible version of `wait()`.

Prototype:

```c
pid_t waitpid(pid_t pid, int *status, int options);
```

### Parameters

* `pid`

  * `> 0` → wait for a specific child
  * `-1` → wait for any child (similar to `wait()`)
* `status`

  * pointer to exit status
  * `NULL` if not needed
* `options`

  * usually `0`
  * can be used for special behavior, e.g. non-blocking wait

Example:

```c
int status;
waitpid(rc, &status, 0);
```

This means:

* wait for child with PID `rc`
* save its exit status into `status`
* block until it finishes

---
### `exec()`

`exec()` is used to run a **different program inside the current process**.

Important:

* `exec()` does **not** create a new process
* it **replaces** the current process image with a new program

So the common pattern is:

* `fork()` creates a child process
* `exec()` is called in the child
* the child stops running the old code and starts running a new program

---

### Main idea

Before `exec()`:

* the current process is running program A

After `exec()`:

* the **same process** is now running program B

So `exec()` does not start an additional process.
It transforms the already existing one.

If `exec()` succeeds:

* the old code is gone
* execution does **not** return to the line after `exec()`

If `exec()` returns, it usually means an error occurred.

---

### What exactly changes after `exec()`

When `exec()` runs successfully, the OS:

* loads code and static data from the new executable
* replaces the old code and old static data
* reinitializes the stack and heap
* starts execution of the new program from its entry point
* passes the provided arguments as `argv[]`

So from the program’s point of view, it is almost as if the old process image never existed.

---

### Why `fork()` + `exec()` are often used together

This is the standard UNIX pattern:

* `fork()` makes a copy of the current process
* the child can then call `exec()` to run another program
* the parent can continue its own work or wait for the child

This is very useful in shells.

Example idea:

* parent process = shell
* child process = command to run
* child calls `exec()` and becomes e.g. `ls`, `wc`, `gcc`, etc.

---

### Variants of `exec()`

There are several versions:

* `execl()`
* `execv()`
* `execlp()`
* `execvp()`
* `execle()`
* `execve()`

The main differences are:

1. **how arguments are passed**
2. **whether the program is searched in `PATH`**
3. **whether a custom environment is provided**

---

### `l` vs `v`

* `l` = **list**

  * arguments are passed one by one

* `v` = **vector**

  * arguments are passed as an array of pointers

Example:

```c
execl("/bin/ls", "ls", "-l", NULL);
```

vs

```c
char *args[] = {"ls", "-l", NULL};
execv("/bin/ls", args);
```

---

### `p`

* `p` = search the program in `PATH`

Example:

```c
execlp("ls", "ls", "-l", NULL);
```

You do not need to write the full path like `/bin/ls`.

---

### `e`

* `e` = provide a custom environment

This is useful when you want the new program to run with your own set of environment variables instead of inheriting the current ones.

---

### Example with `fork()` and `execvp()`

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("hello from process (%d)\n", getpid());

    int rc = fork();

    if (rc < 0) {
        perror("fork");
        exit(1);
    } else if (rc == 0) {
        printf("hello, I'm child process (%d)\n", getpid());

        char *myargs[] = {"wc", "code3-3.c", NULL};
        execvp(myargs[0], myargs);

        // execvp() returns only if an error occurred
        perror("execvp");
        exit(1);
    } else {
        printf("it's process (%d): parent of process (%d)\n", getpid(), rc);
        wait(NULL);
    }

    return 0;
}
```

---

### How this example works

1. The program starts as one process.
2. `fork()` creates a child process.
3. The parent and child both continue from the line after `fork()`.
4. In the child:

   * `execvp("wc", myargs)` is called
   * the child process is replaced by the `wc` program
   * `wc` runs with argument `"code3-3.c"`
5. The parent waits for the child to finish using `wait(NULL)`.

So effectively, the child becomes equivalent to running:

```bash
wc code3-3.c
```

---

### Important note about arguments

In:

```c
char *myargs[] = {"wc", "code3-3.c", NULL};
```

* `myargs[0]` is the program name
* `myargs[1]` is the first argument
* `myargs[2] = NULL` marks the end of the argument array

That final `NULL` is required.

---

### Memory note

For:

```c
char *myargs[] = {"wc", "code3-3.c", NULL};
```

* the array `myargs` itself is a local variable, so it is typically stored on the **stack**
* the string literals `"wc"` and `"code3-3.c"` are stored in static read-only memory
* the **heap is not used** here