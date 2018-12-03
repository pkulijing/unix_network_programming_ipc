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

