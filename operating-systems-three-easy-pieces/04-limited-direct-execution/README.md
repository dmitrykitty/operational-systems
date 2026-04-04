## Mechanism: Limited Direct Execution

To run programs efficiently, the operating system cannot interpret every instruction by itself. That would be far too slow.
Instead, modern systems use a technique called **limited direct execution**.

The main idea is simple:

* the program runs **directly on the CPU**
* but the operating system still keeps **control**
* the program can execute normal instructions by itself
* for privileged operations, it must ask the kernel for help

So the OS gives the process fast execution, but with strict boundaries.

---

## Direct Execution Protocol (Without Limits)

In the simplest model, the workflow looks like this:

### OS side

1. Create an entry for the process in the process list
2. Allocate memory for the program
3. Load the program into memory
4. Set up the stack with `argc` and `argv`
5. Clear registers
6. Execute a call to `main()`

### Program side

1. Run `main()`
2. Execute `return` from `main`

### OS side after program finishes

1. Free memory of the process
2. Remove the process from the process list

This model is fast, because the program runs natively on the processor.
However, it has a serious problem: if the program runs directly on the CPU, how can the OS stop it from doing dangerous things?

For example:

* issuing raw I/O operations
* accessing memory that does not belong to it
* taking over the CPU forever

That is why **direct execution without limits is not enough**.

---

## User Mode and Kernel Mode

To solve this problem, the hardware provides at least two execution modes:

### User mode

This is where normal applications run.

In user mode, the process has **restricted privileges**:

* it cannot directly access devices
* it cannot execute privileged instructions
* it cannot fully control memory management
* it cannot configure interrupt behavior

### Kernel mode

This is where the operating system runs.

In kernel mode, the OS has full access to:

* I/O devices
* page tables and memory-related mechanisms
* interrupt configuration
* CPU control instructions

So the process runs fast in **user mode**, but must switch to **kernel mode** whenever it needs a privileged operation.

---

## Why System Calls Exist

A user program still needs to do real work, and real work often requires OS support.

Examples:

* opening a file
* reading from a file
* writing to a file
* creating a process
* allocating more memory
* sending data through the network

A normal user program is not allowed to do these things directly.
Instead, it uses a **system call**.

A **system call** is the controlled way for a program to request a service from the kernel.

You can think of it like this:

* a normal function call goes from one part of user code to another
* a system call goes from **user space to kernel space**

So a system call is the safe bridge between a process and the OS.

---

## How a System Call Works

Here is the logic step by step.

### 1. The program wants something privileged

For example:

```c
int fd = open("file.txt", O_RDONLY);
```

`open()` looks like a normal C function, but inside it eventually triggers a **system call**.

---

### 2. Arguments are prepared

Before entering the kernel, the program or C library places:

* the **system call number**
* the **arguments**

into predefined places, usually:

* registers
* or the user stack

For example, the kernel must know:

* which syscall is requested (`open`, `read`, `write`, etc.)
* what parameters were passed

---

### 3. Trap instruction is executed

Then the program executes a special CPU instruction often called a **trap**.

This instruction does two crucial things:

* switches CPU execution from **user mode** to **kernel mode**
* transfers control to kernel code

This is not an ordinary jump.
The user process cannot just jump anywhere inside the kernel. That would be extremely unsafe.

The trap is a **protected transfer of control**.

---

### 4. Hardware saves current process state

When the trap happens, the CPU saves enough information to allow returning later.

Typically this includes things like:

* program counter
* flags
* some register state

This saved state is necessary because after the kernel finishes its work, the process must continue exactly where it stopped.

---

### 5. Hardware uses the trap table

Now comes the role of the **trap table**.

At boot time, the kernel sets up a special structure called the **trap table**.
This table tells the CPU:

* what code to run on a system call
* what code to run on a timer interrupt
* what code to run on a keyboard interrupt
* what code to run on disk interrupts
* what code to run on exceptional events like illegal instructions

So when the trap instruction is executed, the CPU does **not** let the process choose the target address.
Instead, the hardware consults the trap table and jumps to the correct **trap handler** registered by the OS.

This is essential for security.

---

## Trap Table

The **trap table** is a data structure configured by the kernel during system boot.

Its job is to map specific events to specific kernel handlers.

Examples of such events:

* system call
* timer interrupt
* page fault
* divide by zero
* illegal instruction
* keyboard interrupt
* disk interrupt

The kernel installs addresses of the corresponding handler functions into this table while running in privileged mode.

That means:

* only the OS can configure it
* user programs cannot replace it
* hardware trusts the trap table because it was installed by the kernel

Without this mechanism, user processes could redirect control flow into arbitrary kernel code, which would completely break system safety.

---

## Trap Handlers

A **trap handler** is kernel code executed after a trap or interrupt occurs.

Examples:

* system call handler
* timer interrupt handler
* keyboard interrupt handler
* page fault handler

The handler decides what to do next.

For a **system call handler**, the OS usually:

1. reads the system call number
2. reads syscall arguments
3. validates them
4. performs the requested operation
5. stores the return value
6. returns control to user mode

So the handler is the real implementation point of the system call inside the kernel.

---

## Returning from the System Call

After the kernel finishes handling the request, it executes a special instruction often called **return-from-trap**.

This operation:

* restores the saved process state
* switches CPU back from **kernel mode** to **user mode**
* resumes execution right after the system call

From the user program’s point of view, it looks almost like a normal function returned.

For example:

```c
int fd = open("file.txt", O_RDONLY);
```

The program simply receives:

* a file descriptor on success
* or `-1` on failure

But internally, a protected transition to the kernel happened and then control came back.

---

## Why a System Call Is Not Just a Normal Function Call

At first glance, something like `read()` or `open()` looks like a normal C function call.

But it is fundamentally different.

### Normal function call

* stays in user space
* does not change CPU privilege level
* just jumps to another function

### System call

* crosses from user space into kernel space
* changes privilege level
* uses hardware trap support
* is handled by the OS through the trap table

So the syntax may look similar, but the mechanism is much deeper.

---

## Why the Kernel Must Validate Everything

A user process is untrusted.

That means the kernel must carefully check:

* whether the syscall number is valid
* whether the pointers passed by the user are valid
* whether the file descriptor exists
* whether the process has permission
* whether memory access is legal

The kernel must never blindly trust user input.

For example, if a process passes a fake pointer to `write()` or `read()`, the kernel must verify it instead of just using it directly.

This is one of the reasons kernel code is so sensitive and important.

---

## Example: `write()`

Suppose the user program does:

```c
write(fd, buffer, 100);
```

The flow is roughly:

1. user code calls `write()`
2. library code prepares syscall number and arguments
3. trap instruction is executed
4. CPU switches to kernel mode
5. hardware jumps to system call handler using the trap table
6. kernel reads syscall number = `write`
7. kernel validates `fd`, `buffer`, and `100`
8. kernel performs the actual write operation
9. kernel stores the return value
10. return-from-trap restores user context
11. program continues in user mode

So one line of code hides a lot of operating-system machinery.

---

## Why Limited Direct Execution Is “Limited”

The technique is called **limited** direct execution because the process is allowed to run directly on the CPU, but only under specific restrictions.

The limits are:

* user mode restrictions
* privileged instructions blocked in user mode
* system calls required for protected operations
* interrupts that let the OS regain control
* trap table controlled only by the kernel

So the process gets speed, but not full power.

That is the balance:

* **performance from direct execution**
* **safety and control from hardware + kernel mechanisms**

---

## Summary

**Limited direct execution** is the basic mechanism used by operating systems to run programs efficiently while still keeping control over the machine.

The process:

* runs directly on the CPU
* runs in **user mode**
* can execute normal instructions quickly
* must use **system calls** for privileged operations

A **system call** works by:

* preparing arguments
* executing a **trap**
* switching into **kernel mode**
* using the **trap table**
* running the correct **trap handler**
* performing the requested kernel operation
* returning back to **user mode**

This mechanism is one of the foundations of modern operating systems because it combines:

* speed
* protection
* controlled access to hardware and kernel services
