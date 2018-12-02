# Record Locking

## Introduction

* read-write lock (ch8): shared among threads in the same process or processes sharing a piece of shared memory
* record locking: share r/w of a file among related or unrelated processes.
    * performed by `fcntl`
    * file referenced through fd
    * owner of file identified by pid
    * maintained within the kernel
* granularity of record locking: size of the object that can be locked
    * the smaller the granularity, the greater the number of simutaneous users allowed
    * Posix record locking: ONE byte

## Posix `fcntl` Record Locking

```c
#include <fcntl.h>
int fcntl(int fd, int cmd, ... /* struct flock *arg */);
```

* 3 values of `cmd` are used with record locking, requiring the 3rd argument `arg` being a pointer to

```c
struct flock {
    short l_type;   /* F_RDLCK, F_WRLCK, F_UNLCK */
    short l_whence; /* SEEK_SET, SEEK_CUR, SEEK_END */
    off_t l_start;  /* relative starting offset in bytes */
    off_t l_len;    /* #bytes. 0 means until end-of-file */
    pid_t l_pid;    /* PID returned by F_GETLK */
}
```

* F_SETLK: obtain (F_RDLCK/F_WRLCK) or release (F_UNLCK) the lock described by `arg` (non blocking,     s EACCES or EAGAIN on failure).
* F_SETLKW: similar to F_SETLK but blocks until lock available (W = waiting)
* F_GETLK: examine the lock pointed to by `arg` to see if an existing lock would prevent a lock from being granted
    * if no lock exists currently: l_type set to F_UNLCK
    * if lock exists: `arg` overridden with information of the existing lock
* F_SETLK followed by F_GETLK is not atomic
* at most one type of lock (r or w) can exist for any byte of a file; a byte can have multiple read locks but only a single write lock
* all locks associated with a file for a given process are removed when 
    * a descriptor for that file is closed by that process
    * when the process holding the descriptor terminates
* locks are not inherited by a forked child process
* record locking should not be used with standard IO library (buffered). Use (unbuffered) `read` and `write` to avoid problems.

## Advisory Locking v.s. Mandatory Locking

* advisory locking: kernel does not prevent a process from writing to a file read-locked by another process; neither does it prevent a process from reading from a file write-locked by another process. i.e. the lock can be ignored by a process
    * fine for cooperating processes
    * defined by Posix
* mandatory locking: `read` and `write` calls are checked to comply with locks
    * supported by some systems
    * turned on for a file by: set group-execute bit off and set-group-ID bit on
* mandatory locking does not solve the problem of uncooperating processes: an read-change-write action of an uncooperating process is not atomic. If a context switch happens during it, followed by an read-change-write action of a cooperating process, the value of the record will be wrong.
* If multiple processes are updating a file, all processes must cooperate using some form of locking. 

## Priorities of Readers and Writers

The priority of readers and writers is not specified by Posix. In our implementation of rdlocks (ch8), we gave priority to waiting writers over waiting readers. But our experiments (`locktest1.c`, `locktest2.c`) shows that ubuntu: 

* grants additional read locks even if there exists waiting locks
* handles waiting locks in simple FIFO manner, regardless of them being read locks or write locks

Such behavior may cause starvation of pending write locks: if new read requests are continually allowed, pending write requests may never be allowed. 

## Application of Record Locking: Start Only One Copy of a Daemon

* Obtain a write lock to a file when starting the daemon (exit if fail)
* Write own pid to the file
* Do what the daemon should do

In case of premature crash: the lock is automatically released by the kernel

