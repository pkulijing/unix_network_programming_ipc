# Mutexes and Condition Variables

* Synchronization
    * how to synchronize actions of multiple threads / processes 
    * needed to allow the sharing of data between threads / processes
    * building blocks: mutexes, condition variables
  
## Mutex: Locking and Unlocking

* Mutex: mutual exclusion
    * used to protect a *critical region* (to make certain that only one thread / process at a time executes the code within the region)
        ```c
        lock_the_mutex(...);
        critical region
        unlock_the_mutex(...);
        ```
    * only one thread / process at a time can lock a given mutex
    * normally used to protect shared data between multiple threads / processes
    * mutex locks are *cooperative* locks (nothing can stop one thread from manipulating the shared data without obtaining the mutex)
* Programming interface
    
    ```c
    #include <pthread.h>
    int pthread_mutex_lock(pthread_mutex_t *mptr);
    int pthread_mutex_trylock(pthread_mutex_t *mptr);
    int pthread_mutex_unlock(pthread_mutex_t *mptr);
    ```

    * statically allocated mutex can be initialized with a constant
    
        ```c
        static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
        ```
    * dynamically allocated mutex or mutex allocated in shared memory must be initialized at run time by calling `pthread_mutex_init`
    * if `*mptr` is already locked
        * `pthread_mutex_lock` blocks until it is unlocked
        * `pthread_mutex_trylock` returns `EBUSY` immediately

## Condition Variables: Waiting and Signaling

* Condition variable: for waiting (v.s. mutex: for locking)
* Programming interface

    ```c
    #include <pthread.h>
    int pthread_cond_wait(pthread_cond_t *cptr, pthread_mutex_t *mptr);
    int pthread_conf_signal(pthread_cond_t *cptr);
    ```

    * A mutex is always associated with a condition variable
    * calling `pthread_cond_wait` does 2 things
        * unlocks the mutex
        * puts the thread to sleep until some other thread calls `pthread_cond_signal` for this condition variable
    * before returning, `pthread_cond_wait` locks the mutex, and the condition is tested again (via the `while` loop) in case of spurious wake-ups 
    * avoiding lock conflicts: the mutex must be locked by the thread calling `pthread_cond_signal` to guarantee predictable scheduling behavior
* programming pattern

    ```c
    struct {
        pthread_mutex_t mutex;
        pthread_cond_t cond;
        // variables maintaining the condition
    } var = {
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_COND_INITIALIZER
    };
    ```

    * code that signals a condition variable

        ```c
        pthread_mutex_lock(&var.mutex);
        // set condition true
        pthread_cond_signal(&var.cond);
        pthread_mutex_unlock(&var.mutex);
        ```
    
    * code that tests the condition variable and waits for the condition to become true
  
        ```c
        pthread_mutex_lock(&var.mutex);
        while (/* condition is false */) {
            pthread_cond_wait(&var.cond, &var.mutex);
        }
        // modify condition
        pthread_mutex_unlock(&var.mutex);
        ```

* Timed Waits and Broadcasts

    ```c
    struct timespec {
        time_t tv_sec;  // seconds
        long   tv_nsec; // nanoseconds
    };
    #include <pthread.h>
    int pthread_cond_broadcast(pthread_cond_t *cptr);
    int pthread_cond_timedwait(pthread_cond_t *cptr, pthread_mutex_t *mptr, 
        const struct timespec *abstime);
    ```
    * `pthread_cond_broadcast` wakes up all threads that are blocked on the condition variable
    * `pthread_cond_timedwait` lets a thread place a limit on how long it will block. The function returns at the specified system time even if the condition variable has not been signaled yet
  
## Mutexex and Condition Variable Attributes

* Initialization & Destruction
    ```c
    #include <pthread.h>
    int pthread_mutex_init(pthread_mutex_t *mptr, const pthread_mutexattr_t *attr);
    int pthread_mutex_destroy(pthread_mutex_t *mptr);
    int pthread_cond_init(pthread_cond_t *cptr, const pthread_condattr_t *attr);
    int pthread_cond_destroy(pthread_cond_t *cptr);
    ```
* `pthread_mutexattr_t` and `pthread_condattr_t` are initialized & destroyed in a similar manner (with `pthread_xxxx_init` and `pthread_xxxx_destroy` functions)
* The attribute that specifies whether the mutex / condition variable is to be shared between different processes (not just between threads): `PTHREAD_PROCESS_PRIVATE` or `PTHREAD_PROCESS_SHARED`
    * The mutex / condition variable must be stored in shared memory
    * There is no way to have the system automatically release held locks upon process termination. Even if there were such a way, problem might still exist: corrupted shared data which is meant to be protected by the mutex / condition variable
  
    ```c
    #include <pthread.h>
    int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *valptr);
    int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int value);
    // condattr similar
    ```
