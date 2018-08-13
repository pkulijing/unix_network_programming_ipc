#ifndef _my_pthread_rwlock_h_
#define _my_pthread_rwlock_h_
#include <pthread.h>
typedef struct {
    pthread_mutex_t rw_mutex;
    pthread_cond_t rw_condreaders;
    pthread_cond_t rw_condwriters;
    int rw_magic; // for err checking
    int rw_nwaitreaders;
    int rw_nwaitwriters;
    int rw_refcount; // -1 for writer, 0 for available, # readers for readers
} my_pthread_rwlock_t;

#define RW_MAGIC 0x19283746
#define MY_PTHREAD_RWLOCK_INITIALIZER { PTHREAD_MUTEX_INITIALIZER, \
    PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, RW_MAGIC, 0, 0, 0 }

typedef int my_pthread_rwlockattr_t; // not supported 

int my_pthread_rwlock_init(my_pthread_rwlock_t *, my_pthread_rwlockattr_t *);
int my_pthread_rwlock_destroy(my_pthread_rwlock_t*);
int my_pthread_rwlock_rdlock(my_pthread_rwlock_t*);
int my_pthread_rwlock_wrlock(my_pthread_rwlock_t*);
int my_pthread_rwlock_unlock(my_pthread_rwlock_t*);
int my_pthread_rwlock_tryrdlock(my_pthread_rwlock_t*);
int my_pthread_rwlock_trywrlock(my_pthread_rwlock_t*);

#endif // _my_pthread_rwlock_h_

