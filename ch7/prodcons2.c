#include <pthread.h>
#include <utils.h>
#include <stdlib.h>

#define MAX_ITEMS 1000000
#define MAX_THREADS 64

typedef struct shared_data_STRUCTDEF {
    pthread_mutex_t mutex;
    int buf[MAX_ITEMS];
    int nput; // next position to put an item
    int nval; // next val to put
} shared_data;

shared_data shared = {
    PTHREAD_MUTEX_INITIALIZER
};

int nitems; // number of items 

void *producer (void *arg) {
    while (1) {
        pthread_mutex_lock(&shared.mutex);
        if (shared.nput >= nitems) {
            pthread_mutex_unlock(&shared.mutex);
            break;
        }
        shared.buf[shared.nput++] = shared.nval++;
        pthread_mutex_unlock(&shared.mutex);
        (*(int*)arg) += 1;
    }
    return NULL;
}

void *consumer(void *arg) {
    int i;
    for (i = 0; i < nitems; ++i) {
        ASSERT_ERR_SYS(i == shared.buf[i], "buf[%d] = %d\n", i, shared.buf[i]);
    }
    printf("consumer finished!");
    return NULL;
}

int main(int argc, char** argv) {
    int nthreads, i, count[MAX_THREADS];
    pthread_t tid_producer[MAX_THREADS], tid_consumer;

    ASSERT_ERR_QUIT(3 == argc, "usage: prodcons2 [nitems] [nthreads]");

    nitems = min(atoi(argv[1]), MAX_ITEMS);
    nthreads = min(atoi(argv[2]), MAX_THREADS);

    printf("nitems = %d, nthreads = %d\n", nitems, nthreads);

    ASSERT_ERR_QUIT(nitems > 0 && nitems <= MAX_ITEMS, "nitems = %d", nitems);
    ASSERT_ERR_QUIT(nthreads > 0 && nthreads <= MAX_THREADS, 
        "nthreads = %d", nthreads);

    for (i = 0; i < nthreads; ++i) {
        count[i] = 0;
        ASSERT_ERR_QUIT(0 == pthread_create(&tid_producer[i], NULL,
            &producer, &count[i]), "Failed to create thread %d", i);
    }

    for (i = 0; i < nthreads; ++i) {
        ASSERT_ERR_QUIT(0 == pthread_join(tid_producer[i], NULL), 
            "Failed to join thread %d(tid = %ld)", i, (long)(tid_producer[i]));
    }

    int sum = 0;
    for (i = 0; i < nthreads; ++i) {
        printf("tid_producer[i] = %ld, count[i] = %d\n", (long)(tid_producer[i]), count[i]);
        sum += count[i];
    }
    printf("sum = %d, nitems = %d\n", sum, nitems);

    ASSERT_ERR_QUIT(0 == pthread_create(&tid_consumer, NULL, &consumer, NULL),
        "Failed to create consumer thread");

    ASSERT_ERR_QUIT(0 == pthread_join(tid_consumer, NULL), "Failed to join "
        "consumer thread (tid = %ld)", (long)tid_consumer);
    
    exit(0);
}