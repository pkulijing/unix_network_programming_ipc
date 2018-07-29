## Pipes & FIFOs
### Pipes
* No name. Usable only by related processes
* Accessed using `read` and `write` functions
* Created by the `pipe` function
    ```c
    #include <unistd.h>
    int pipe(int fd[2]);
    ```
* Provides one-way (unidirectional) flow of data
* Returns two descriptors: `fd[0]` open for reading, `fd[1]` open for writing 
* Typical usage
    - Parent creates a pipe
    - Parent calls `fork`
    - Parent closes read end, child closes write end, ending up with a one-way flow of data from parent to child
* Usage in shell

    ```bash
    who | sort | lp
    ```
    
    - In the example above, shell creates 2 pipes between 3 processes: who -> sort -> lp
    - For each pipe, shell duplicates read end to standard input and write end to standard output
* For two-way flow of data, two pipes are needed
    - Parent creates pipe 1 and pipe 2
    - Parent calls `fork`
    - Parent closes read end of pipe 1 and write end of pipe 2 (`fd1[0]` and `fd2[1]`)
    - Child closes write end of pipe 1 and read end of pipe 2 (`fd[1]` and `fd2[0]`)
* Full-duplex pipes (both file descriptors readable and writable) are constructed from two half-duplex pipes. What written to `fd[0]` is readable with `fd[1]` and vice versa.
* `stdio` support

    ```c
    #include <stdio.h>
    FILE *popen(const char *command, const char *type);
    int pclose(FILE *stream);
    ```
    
    - `popen` creates a pipe and initiates another process that reads from or writes to the pipe
    - `command` is a shell command
    - the `FILE*` returned by `popen` is used for read or write
        + If `type` is `r`: calling process reads from the standard output of `command`
        + If `type` is `w`: calling process writes to the standard iutput of `command`
    - `pclose` closes the IO stream created by `popen`, waits for the command 
    to terminate and returns the termination status of the shell

### FIFOs (First In First Out, or Named Pipes)
* One way flow of data, similar to a pipe
* Has a pathname associated with it, allowing unrelated processes to acccess a single FIFO
* Created by calling `mkfifo` function or shell command.
    ```c
    #include <sys/types.h>
    #include <sys/stat.h>
    int mkfifo(const char *pathname, mode_t mode);
    ```
    - `mkfifo` implies `O_CREAT | O_EXCL`: create and returns `EEXIST` if the FIFO already exists
    - Must be opened for reading or reading using `open` or standard IO open functions, e.g. `fopen`
    - Musted be opened read-only or write only, NOT read-write
    - `write` to a pipe of fifo always appends the data, and a `read` always returns what is at the beginning
    - `lseek` cannot be called for pipe or FIFO (returning `ESPIPE`)
#### Additional Properties
* The `O_NONBLOCK` flag can be specified with `open` (only for fifo) or `fcntl`
    ```c
    int flags;
    flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
    ```
* If the number of bytes to `write` is smaller than or equal to `PIPE_BUF` (at least 512B as per Posix, usually 1024B - 5120B), the write is guaranteed to be atomic
* System imposed limits
    - `OPEN_MAX`: maximum number of descriptors open at any time by a process (at least 16 as per Posix)
    - `PIPE_BUF`: maximum amount of data that can be written to a pipe or fifo atomically

