#include "my_pthread_rwlock.h"
#include <unistd.h>
#include <stdio.h>
my_pthread_rwlock_t rwlock = MY_PTHREAD_RWLOCK_INITIALIZER;
pthread_t tid1, tid2;
void *thread1(void *);
void *thread2(void *);

int main(int argc, char **argv) {
    void *status;

    pthread_create(&tid1, NULL, thread1, NULL);
    sleep(1);
    pthread_create(&tid2, NULL, thread2, NULL);
    pthread_join(tid2, &status);
    if (status != PTHREAD_CANCELED) {
        printf("thread2 status: %p\n", status);
    }
    pthread_join(tid1, &status);
    if (status != NULL) {
        printf("thread1 status: %p\n", status);
    }

    printf("rw_refcount = %d, rw_nwaitreaders = %d, rw_nwaitwriters = %d\n",
      rwlock.rw_refcount, rwlock.rw_nwaitreaders, rwlock.rw_nwaitwriters);
    my_pthread_rwlock_destroy(&rwlock);
    return 0;
}

void *thread1(void *arg) {
    my_pthread_rwlock_rdlock(&rwlock);
    printf("thread1 got a read lock\n");
    sleep(3);
    pthread_cancel(tid2);
    sleep(3);
    my_pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void *thread2(void *arg) {
    printf("thread2 trying to get a write lock\n");
    my_pthread_rwlock_wrlock(&rwlock);
    printf("thread2 got a write lock\n");
    sleep(1);
    my_pthread_rwlock_unlock(&rwlock);
    return NULL;
}