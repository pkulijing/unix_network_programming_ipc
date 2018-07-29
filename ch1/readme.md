## Introduction
IPC: InterProcess Communication
### Evolution of Message Passing
* Pipes
    - within the program or from the shell
    - problem: only between processes with common ancestor (fixed by named pipes / FIFOs)
* System V Message Queues & Posix Message Queues
    - used between related or unrelated processes on a given host
* Remote Procedure Calls (RPCs)
    - call a function on one system (server) from a program on another system (client)
    - alternative to explicit network programming
    - information passing & can be used on the same host => another form of message passing
### Evolution  of Synchronization
* Early programs
    - often for preventing simultaneous modification of a file
    - used quirks of the filesystem
* Record locking
* System V semaphores & System V shared memory
* Posix semaphores & Posix shared memory
* Mutexes and condition variables
    - often used for thread synchronization
    - can provide process synchronization
* Read-write locks

## Processes, Threads and the Sharing of Information
* Traditional Unix programming model
    - multiple processes running on a system
    - each process has its own address space
    - processes share information in various ways
### Different ways to share information
* Information in file
    - each process has to go through the kernel (e.g. read, write, lseek) for access
    - synchronization required when file is being updated: protect writes from each other, protect readers from a writer
* Information in the kernel
    - e.g. pipe, System V message queues, System V semaphores
    - system call into the kernel required for access
* Information in shared memory
    - access to information without involving kernel once setup
    - requires some form of synchronization

### Threads
* From an IPC point of view
    - all threads within the same process share the same global variables (e.g. shared memory concept inherent to this model)
    - access to global data must be synchronized
* Problems with `fork`
    - `fork` is expensive
        + memory copied from parent to child
        + descriptors duplicated in child
        + copy-on-write for optimization
        + thread creation: 10 - 100 times faster
    - IPC difficulty
        + information from parent to child: easy
        + returning data from child to parent takes some work
* Threads within the same process share
    - process instructions
    - most data
    - open files
    - signal handlers and signal dispositions
    - current working directory
    - user & group IDs
* Each thread has its own
    - thread ID
    - set of registers, including PC & stack pointer
    - stack (local variables & return addresses)
    - `errno`
    - signal mask
    - priority
* Basic thread functions

    ```c
    #include <pthread>
    // create a new thread ~ fork
    int pthread_create(pthread_t *tid, 
                    const pthread_attr_t *attr,
                    void *(*func)(void *),
                    void *arg);
    // wait for a thread to terminate ~ waitpid
    int pthread_join(pthread_t tid, void** status);
    // get tid within thread itself ~ getpid
    pthread_t pthread_self(void);
    // change a thread from joinable to detached
    int pthread_detach(pthread_t tid);
    // terminate a thread within itself ~ exit
    void pthread_exit(void* status);
    ```

### Persistence of IPC Objects
* Process persistent
    - exists until the last process holding IPC object open closes it
    - e.g. FIFO, pipe, TCP/UDP socket, Unix domain socket
* Kernel persistent
    - exists until kernel reboot or object deletion
    - e.g. message queues, semaphores, shared memory
* Filesystem persistent
    - exists until object deletion
    - e.g. posix message queues, semaphores, shared memory if implemented using mapped files (not required)
