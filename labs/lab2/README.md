# Lab 2 — Files and Processes

## Topics covered

- file descriptors
- `open()`, `read()`, `write()`, `close()`
- processing files byte by byte
- UTF-8 byte handling (basic practical case)
- opening and reading directories
- `opendir()`, `readdir()`, `closedir()`
- checking file type with `dirent`
- building full paths with `snprintf()`
- process creation with `fork()`
- waiting for child processes with `wait()`
- replacing file contents through a temporary file and `rename()`

---

## 1. File task

### Goal

Read a file given as a program argument and replace Polish characters with their English equivalents.

Examples:

- `ą -> a`
- `ć -> c`
- `ę -> e`
- `ł -> l`
- `ń -> n`
- `ó -> o`
- `ś -> s`
- `ź -> z`
- `ż -> z`

The program writes the result to a new file: `output.txt`.

---

### APIs used

- `open()` — open file for reading or writing
- `read()` — read bytes from a file
- `write()` — write bytes to a file
- `close()` — close file descriptor

---

### Important idea

The program works directly on **bytes**, not on high-level strings.

ASCII characters are 1 byte:

- if byte `< 128`, it is copied directly

Polish characters in UTF-8 are stored as **two bytes**, so the program:

1. reads the first byte
2. if it is non-ASCII, reads the second byte
3. checks if the pair matches a known Polish character
4. writes either:
   - converted English letter
   - or original bytes if it is some other non-ASCII character

---

### Example flow

```c
int in_file = open(input_file, O_RDONLY);
int out_file = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
````

* input file is opened read-only
* output file is created if needed
* `O_TRUNC` clears the file before writing
* `0644` means standard text-file permissions

---

### What to remember

* `open()` returns a file descriptor
* `read()` returns:

  * `> 0` number of bytes read
  * `0` EOF
  * `-1` error
* `write()` should be checked to make sure writing succeeded
* file descriptors should always be closed with `close()`

---

## 2. Process task

### Goal

Read a directory given as a program argument.
For every `.txt` file inside that directory:

* create a separate child process
* convert Windows line endings (`CRLF`) to UNIX line endings (`LF`)

Each file is processed by its own child process.

---

### APIs used

#### Directory handling

* `opendir()` — open a directory stream
* `readdir()` — read directory entries one by one
* `closedir()` — close directory stream

#### Process handling

* `fork()` — create child process
* `wait()` — wait for all child processes to finish
* `_exit()` — terminate child process safely

#### File handling

* `open()`
* `read()`
* `write()`
* `close()`
* `rename()`

#### Other helpers

* `strcmp()` — compare names / extensions
* `strlen()` — check filename length
* `snprintf()` — build full file path safely

---

### How directory reading works

The program opens the directory:

```c
DIR *entity_stream = opendir(catalog_name);
```

Then reads entries one by one:

```c
while ((entity = readdir(entity_stream)) != NULL) {
```

Each entry is checked:

* skip `.` and `..`
* skip non-regular files
* skip files without `.txt` extension

This part is used to make sure only normal `.txt` files are processed.

---

### Building full path

Because `readdir()` gives only entry names, the program builds full path manually:

```c
snprintf(full_path, PATH_SIZE, "%s/%s", catalog_name, entity_name);
```

That gives something like:

```text
folder/file.txt
```

---

### `fork()` in this task

For every valid `.txt` file:

```c
pid_t pid = fork();
```

Meaning:

* parent continues scanning directory
* child processes one file

Pattern:

* `pid < 0` → error
* `pid == 0` → child
* `pid > 0` → parent

This is a good example of using processes for parallel file processing.

---

### Why `_exit()` in child

Child ends with:

```c
_exit(result);
```

instead of `return` or `exit()`.

Why:

* `_exit()` immediately terminates the child
* it avoids some problems with buffered stdio inherited after `fork()`

For child error paths, `_exit()` is usually safer.

---

### How CRLF -> LF conversion works

The child opens input file and creates a temporary file:

```c
int in = open(file_path, O_RDONLY);
int out = open(temp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
```

Then it reads byte by byte.

Main rule:

* if current byte is `'\r'`

  * read next byte
  * if next byte is `'\n'`, write only `'\n'`
* otherwise write original byte

So:

* Windows ending: `\r\n`
* UNIX ending: `\n`

---

### Why temporary file + `rename()`

The program does not overwrite the original file directly while reading it.

Instead:

1. reads original file
2. writes converted content to `file.txt.tmp`
3. after success, replaces original with:

```c
rename(temp_path, file_path);
```

This is safer and simpler than modifying the same file in place.

---

### What to remember

* `opendir()` / `readdir()` / `closedir()` are basic directory APIs
* `readdir()` returns entries one by one
* `fork()` is useful when each file can be processed independently
* parent can create many children and then wait for all of them
* `wait(NULL)` in a loop is a common way to wait for all child processes
* using a temporary file plus `rename()` is a practical pattern for file rewriting

---

```md


Najważniejsza różnica względem poprzedniej wersji jest taka, że `lab2` README teraz odzwierciedla dokładnie to, co masz w kodzie: pliki, katalogi, `fork()`, `wait()`, `_exit()`, CRLF→LF, temp file + `rename()`, bez wciskania tam `exec()`, `pipe()` czy sygnałów, których w labie faktycznie nie użyłeś. To wynika bezpośrednio z zawartości `labs/lab2/files/main.c`, `labs/lab2/processes/task/main.c` i `labs/lab2/processes/tests/main.c`. :contentReference[oaicite:2]{index=2}

Jeżeli chcesz, mogę teraz przygotować jeszcze **krótszą, bardziej „ładną GitHubowo” wersję** README dla `lab2`, trochę bliżej stylu twojego `lab1/README.md`.
::contentReference[oaicite:3]{index=3}
```

[1]: https://github.com/dmitrykitty/operating-systems/tree/main/labs/lab2 "operating-systems/labs/lab2 at main · dmitrykitty/operating-systems · GitHub"
[2]: https://github.com/dmitrykitty/operating-systems "GitHub - dmitrykitty/operating-systems · GitHub"
