# Posix Semaphores

## Introduction

Semaphore provides synchronization between various processes or between threads in a given process. 

* Posix named semaphores: identified by Posix IPC names
* Posix memory-based semaphores: stored in shared memory
* System V semaphores: maintained in the kernel

Operations that a process can perform on a semaphore:

* *create* a semaphore: the caller should specify the initial value (0/1 for a binary semaphore)
* *wait* for a semaphore (P operation): tests the value of the semaphore, waits (blocks) if its value is <= 0, and decrements its value once it's > 0.

  ```c
  while (semaphore_value <= 0) {
    /* wait (block the thread of process) */
  }
  semaphore_value--;
  /* now we have the semaphore */
  ```

  The value test in the while loop and the decrement must be ab *atomic* operation.

* *Post* to a semaphore (V operation).  

  ```c
  semaphore_value++;
  ```

  If other processes are blocked waiting for this semaphore's value to be > 0, one of them can be awoken. The post operation must also be atomic.

A binary semaphore can be used for mutual exclusion like a mutex.

<table>
<thead>
<tr>
<th>mutex / condition variable</th><th>semaphore</th>
</tr>
</thead>
<tbody>
<tr>
<td>initialize mutex;<br>
pthread_mutex_lock(&mutex);<br>
critical region <br>
pthread_mutex_unlock(&mutex);</td>
<td>initialize sem = 1; <br>
sem_wait(&sem); <br>
critical region <br>
sem_post(&sem);</td>
</tr>
<tr>
<td>A mutex must always be unlocked by the thread that locked it</td>
<td>A semaphore post does not have to be performed by the process that did the wait</td>
</tr>
<tr>
<td>A mutex must be either locked or unlocked (binary state)</td>
<td>binary semaphore or counting semaphore</td>
</tr>
<tr>
<td>No state: condition variable signal lost if no thread is waiting for it</td>
<td>Has a state associated with it (value). <i>sem_post</i> is always remembered.</td>
</tr>
</tbody>
</table>

## Posix Named Semaphore Functions
```c
#include <semaphore.h>
// oflag = 0, O_CREAT or OCREAT | O_EXCL. 3rd and 4th args required when O_CREAT is specified
// returns SEM_FAILED on error.
sem_t *sem_open(const char* name, int oflag, ...
  /* mode_t mode, unsigned_int value */ );

// semaphore closed automatically on process termination. Closing a semaphore does not remove it from the system: kernel persistent
int sem_close(sem_t *sem);

// semaphores are reference counted. Not destroyed until the last sem_close occurs.
int sem_unlink(const char *name);

// blocking. returns EINTR prematurely when interrupted by a signal
int sem_wait(sem_t *sem);

// non blocking: EAGAIN on failure.
int sem_trywait(sem_t *sem);

// can be called from a signal handler
int sem_post(sem_t *sem);

// when locked: returns 0 or negative number whose abs = number of threads waiting for  the semaphore to be unlocked
int sem_getvalue(sem_t *sem, int *valp);
```

## Producer-Consumer Problem Using Semaphores

### Circular Buffer

After the producer fills the finay entry `buff[NBUFF - 1]`, it goes back and fills the first entry `buff[0]`, and the consumer does the same. Conditions must be maintained:

* Consumer cannot remove an item from the buffer when it's empty
* Producer cannot place an item into the buffer when it's full
* Buffer manipulations by producer and consumer must be protected to avoid race conditions

### Solution Using Semaphore

* A binary semaphore `mutex` to protect the critical regions (insertion & deletion). Could be replaced by a real mutex.
* A counting semaphore `nempty` to count the number of empty slots. Initialized to `NBUFF`.
* A counting semaphore `nstored` to count the number of filled slots. Initialzed to 0.

### Multiple Buffers

Application of the model: double buffering to accelerate text processing programs of the form:

``` c
while ((n = read(fdin, buff, BUFFSIZE)) > 0) {
  /* process buffer data */
  write(fdout, buff, n);
}
```
Using two threads respectively for reading and writing does not accelerate the program: the writing thread has to wait util the reading thread finishes filling the buffer. Yet adding a second buffer can make it run as fast as possible: the reading thread just switches to the 2nd buffer when the writing thread is processing the 1st buffer.

## Posix Memory-Based Semaphore Functions

``` c
// shared = 0: shared between threads of a process; else: shared between processes (must be stored in shared memory)
// UB if sem_init is called on a sem_t already initialized.
// return -1 on error. DOES NOT return 0 on success!
int sem_init(sem_t *sem, int shared, unsigned int value);
int sem_destroy(sem_t * sem);
```

## Sharing Semaphores between Processes

* memory-based semaphores: resides in shared memories; 2nd arg of `sem_init` must be 1
* named semaphores: different processes can always reference the same semaphore with the same name. Special case with `fork`: semaphores open in the parent process is also open in the child process
