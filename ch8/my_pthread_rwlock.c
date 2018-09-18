#include <errno.h>
#include "utils.h"
#include "my_pthread_rwlock.h"
static void my_rwlock_cancelrdwait(void *arg) {
    my_pthread_rwlock_t *rw;
    rw = arg;
    rw->rw_nwaitreaders -= 1;
    pthread_mutex_unlock(&rw->rw_mutex);
}

static void my_rwlock_cancelwrwait(void *arg) {
    my_pthread_rwlock_t *rw;
    rw = arg;
    rw->rw_nwaitwriters -= 1;
    pthread_mutex_unlock(&rw->rw_mutex);
}
int my_pthread_rwlock_init(my_pthread_rwlock_t* rwptr, my_pthread_rwlockattr_t* attr) {
    int result;

    if (NULL != attr) {
        return EINVAL;
    }

    if ((result = pthread_mutex_init(&rwptr->rw_mutex, NULL)) != 0) {
        goto err1;
    }
    if ((result = pthread_cond_init(&rwptr->rw_condreaders, NULL)) != 0) {
        goto err2;
    }
    if ((result = pthread_cond_init(&rwptr->rw_condwriters, NULL)) != 0) {
        goto err3;
    }
    rwptr->rw_nwaitreaders = 0;
    rwptr->rw_nwaitwriters = 0;
    rwptr->rw_refcount = 0;
    rwptr->rw_magic = RW_MAGIC;

    return 0;

err3:
    pthread_cond_destroy(&rwptr->rw_condreaders);
err2:
    pthread_mutex_destroy(&rwptr->rw_mutex);
err1:
    return result;
}

int my_pthread_rwlock_destroy(my_pthread_rwlock_t *rw) {
    if (rw->rw_magic != RW_MAGIC) {
        return EINVAL;
    }

    if (rw->rw_refcount != 0 || rw->rw_nwaitreaders != 0 || 
        rw->rw_nwaitwriters != 0) {
        return EBUSY;
    }

    pthread_mutex_destroy(&rw->rw_mutex);
    pthread_cond_destroy(&rw->rw_condreaders);
    pthread_cond_destroy(&rw->rw_condwriters);
    rw->rw_magic = 0;
    
    return 0;
}

int my_pthread_rwlock_rdlock(my_pthread_rwlock_t *rw) {
    int result;

    if (rw->rw_magic != RW_MAGIC) {
        return EINVAL;
    }

    if ((result = pthread_mutex_lock(&rw->rw_mutex)) != 0) {
        return result;
    }

    while (-1 == rw->rw_refcount || rw->rw_nwaitwriters > 0) {
        rw->rw_nwaitreaders += 1;
        pthread_cleanup_push(&my_rwlock_cancelrdwait, rw);
        result = pthread_cond_wait(&rw->rw_condreaders, &rw->rw_mutex);
        pthread_cleanup_pop(0);
        rw->rw_nwaitreaders -= 1;
        if (result != 0) {
            break;
        }
    }

    if (result == 0) {
        rw->rw_refcount += 1;
    }

    pthread_mutex_unlock(&rw->rw_mutex);

    return result;
}

int my_pthread_rwlock_tryrdlock(my_pthread_rwlock_t *rw) {
    int result;

    if (rw->rw_magic != RW_MAGIC) {
        return EINVAL;
    }

    if ((result = pthread_mutex_lock(&rw->rw_mutex)) != 0) {
        return result;
    }

    if (-1 == rw->rw_refcount || rw->rw_nwaitwriters > 0) {
        result = EBUSY;
    } else {
        rw->rw_refcount += 1;
    }

    pthread_mutex_unlock(&rw->rw_mutex);

    return result;
}

int my_pthread_rwlock_wrlock(my_pthread_rwlock_t *rw) {
    int result;

    if (rw->rw_magic != RW_MAGIC) {
        return EINVAL;
    }

    if ((result = pthread_mutex_lock(&rw->rw_mutex)) != 0) {
        return result;
    }

    while (rw->rw_refcount != 0) {
        rw->rw_nwaitwriters += 1;
        pthread_cleanup_push(&my_rwlock_cancelwrwait, rw);
        result = pthread_cond_wait(&rw->rw_condwriters, &rw->rw_mutex);
        pthread_cleanup_pop(0);
        rw->rw_nwaitwriters -= 1;
        if (result != 0) {
            break;
        }
    }

    if (result == 0) {
        rw->rw_refcount = -1;
    }
    pthread_mutex_unlock(&rw->rw_mutex);
    return result;
}

int my_pthread_rwlock_trywrlock(my_pthread_rwlock_t *rw) {
    int result;

    if (rw->rw_magic != RW_MAGIC) {
        return EINVAL;
    }

    if ((result = pthread_mutex_lock(&rw->rw_mutex)) != 0) {
        return result;
    }

    if (rw->rw_refcount != 0) {
        result = EBUSY;
    } else {
        rw->rw_refcount = -1;
    }

    pthread_mutex_unlock(&rw->rw_mutex);
    return result;
}

int my_pthread_rwlock_unlock(my_pthread_rwlock_t *rw) {
    int result;
    if (rw->rw_magic != RW_MAGIC) {
        return EINVAL;
    }

    if ((result = pthread_mutex_lock(&rw->rw_mutex)) != 0) {
        return result;
    }

    if (rw->rw_refcount > 0) {
        rw->rw_refcount -= 1;
    } else if (rw->rw_refcount == -1) {
        rw->rw_refcount += 1;
    } else {
        err_sys("rw->rw_refcount = %d", rw->rw_refcount);
    }

    if (rw->rw_nwaitwriters > 0) {
        if (rw->rw_refcount == 0) {
            result = pthread_cond_signal(&rw->rw_condwriters);
        }
    } else if (rw->rw_nwaitreaders > 0) {
        result = pthread_cond_broadcast(&rw->rw_condreaders);
    }

    pthread_mutex_unlock(&rw->rw_mutex);
    return result;
}