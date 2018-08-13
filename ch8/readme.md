# Read-Write Locks

## Introduction

* A _read-write lock_ distinguishes between obtaining it for reading and writing
    * Any number of threads can hold a given read-write lock for reading as long as no thread holds it for writing
    * A read-write lock can be allocated for writing only if no thread holds it for reading or writing
* Also called *shared-exclusive locking* / *readers-writer lock*
    * for reading: a shared lock
    * for writing: an exclusive lock
* Can be implemented with mutex and condition variable
  
## Programming Interface

```c
#include <pthread.h>
int pthread_rwlock_rdlock(pthread_rwlock_t *rwptr);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwptr);
int pthread_rwlock_unlock(pthread_rwlock_t *rwptr);

// Non-blocking: EBUSY on error
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwptr);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwptr);

int pthread_rwlock_init(pthread_rwlock_t* rwptr, const pthread_rwlockattr_t *attr);
int pthread_rwlock_destroy(pthread_rwlock_t* rwptr);

int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);

// getpshared / setpshared also works for rwlocks
```
## Thread Cancellation


