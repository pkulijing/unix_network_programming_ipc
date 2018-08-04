#include <pthread.h>
#include <utils.h>
#include <stdlib.h>

#define MAX_ITEMS 1000000
#define MAX_THREADS 64

typedef struct shared_data_put_STRUCTDEF {
    pthread_mutex_t mutex;
    int nput; // next position to put an item
    int nval; // next val to put
} shared_data_put;

typedef struct shared_data_ready_STRUCTDEF {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int nready; // number of items ready for consumer to process
} shared_data_ready;

shared_data_put put = {
    PTHREAD_MUTEX_INITIALIZER
};

shared_data_ready ready = {
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_COND_INITIALIZER
};

int buf[MAX_ITEMS];

int nitems; // number of items 

void *producer (void *arg) {
    while (1) {
        pthread_mutex_lock(&put.mutex);
        if (put.nput >= nitems) {
            pthread_mutex_unlock(&put.mutex);
            break;
        }
        buf[put.nput++] = put.nval++;
        pthread_mutex_unlock(&put.mutex);

        pthread_mutex_lock(&ready.mutex);
        if (ready.nready == 0) {
            pthread_cond_signal(&ready.cond);
        }
        ready.nready++;
        pthread_mutex_unlock(&ready.mutex);
        
        (*(int*)arg) += 1;
    }
    return NULL;
}

void *consumer(void *arg) {
    int i;
    for (i = 0; i < nitems; ++i) {
        pthread_mutex_lock(&ready.mutex);
        while (ready.nready == 0) {
            pthread_cond_wait(&ready.cond, &ready.mutex);
        }
        ready.nready--;
        pthread_mutex_unlock(&ready.mutex);
        ASSERT_ERR_SYS(i == buf[i], "buf[%d] = %d", i, buf[i]);
    }
    printf("consumer finished!\n");
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

    ASSERT_ERR_QUIT(0 == pthread_create(&tid_consumer, NULL, &consumer, NULL),
        "Failed to create consumer thread");

    for (i = 0; i < nthreads; ++i) {
        ASSERT_ERR_QUIT(0 == pthread_join(tid_producer[i], NULL), 
            "Failed to join thread %d(tid = %ld)", i, (long)(tid_producer[i]));
    }

    int sum = 0;
    for (i = 0; i < nthreads; ++i) {
        printf("tid_producer[%d] = %ld, count[%d] = %d\n", i, (long)(tid_producer[i]), i, count[i]);
        sum += count[i];
    }

    printf("sum = %d, nitems = %d\n", sum, nitems);

    ASSERT_ERR_QUIT(0 == pthread_join(tid_consumer, NULL), "Failed to join "
        "consumer thread (tid = %ld)", (long)tid_consumer);
    
    exit(0);
}