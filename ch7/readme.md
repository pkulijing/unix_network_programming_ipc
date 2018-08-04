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
