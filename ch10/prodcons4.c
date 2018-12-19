#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include "utils.h"

void *producer(void *);
void *consumer(void *);

#define NBUFF 10
#define MAXTHREADS 100

int nitems;
int nproducers;
int nconsumers;

struct {
  int buff[NBUFF];
  int nput;
  int nputval;
  int nget;
  int ngetval;
  sem_t mutex, nempty, nstored;
} shared;

int main(int argc, char** argv) {
  pthread_t tid_producer[MAXTHREADS], tid_consumer[MAXTHREADS];
  int i, count_p[MAXTHREADS], count_c[MAXTHREADS];

  ASSERT_ERR_QUIT(argc == 4, "usage: %s [#items] [#producers] [#consumers]", argv[0]);

  nitems = atoi(argv[1]);
  nproducers = atoi(argv[2]);
  nconsumers = atoi(argv[3]);

  ASSERT_ERR_QUIT(sem_init(&shared.mutex, 0, 1) != -1, "Failed to init mutex: %d", errno);
  ASSERT_ERR_QUIT(sem_init(&shared.nempty, 0, NBUFF) != -1, "Failed to init nempty: %d", errno);
  ASSERT_ERR_QUIT(sem_init(&shared.nstored, 0, 0) != -1, "Failed to init nstored: %d", errno);

  shared.nput = 0;
  shared.nputval = 0;  
  shared.nget = 0;
  shared.ngetval = 0;

  for (i = 0; i < nproducers; ++i) {
    ASSERT_ERR_QUIT(pthread_create(&tid_producer[i], NULL, &producer, &count_p[i]) == 0, 
      "Failed to create producer thread");
  }
  for (i = 0; i < nconsumers; ++i) {
    ASSERT_ERR_QUIT(pthread_create(&tid_consumer[i], NULL, &consumer, &count_c[i]) == 0,
      "Failed to create consumer thread");
  }
  int total_p = 0, total_c = 0;

  for (i = 0; i < nproducers; ++i) {
    ASSERT_ERR_QUIT(pthread_join(tid_producer[i], NULL) == 0, 
      "Failed to join producer thread: %ld", tid_producer[i]);
    total_p += count_p[i];
    printf("count_p[%d] = %d\n", i, count_p[i]);
  }
  for (i = 0; i < nconsumers; ++i) {
    ASSERT_ERR_QUIT(pthread_join(tid_consumer[i], NULL) == 0, 
      "Failed to join consumer thread: %ld", tid_consumer[i]);
    total_c += count_c[i];
    printf("count_c[%d] = %d\n", i, count_c[i]);
  }

  ASSERT_ERR_QUIT(0 == sem_destroy(&shared.mutex), "Failed to destroy semaphore mutex");
  ASSERT_ERR_QUIT(0 == sem_destroy(&shared.nempty), "Failed to destroy semaphore nempty");
  ASSERT_ERR_QUIT(0 == sem_destroy(&shared.nstored), "Failed to destroy semaphore nstored");

  ASSERT_ERR_QUIT(total_p == nitems, "total_p = %d, nitems = %d", total_p, nitems);
  ASSERT_ERR_QUIT(total_c == nitems, "total_c = %d, nitems = %d", total_c, nitems);

  return 0;
}

void * producer(void * arg) {
  int nextval = 0;

  *(int*)arg = 0;
  
  while (nextval < nitems) {
    ASSERT_ERR_QUIT(sem_wait(&shared.nempty) == 0, "Failed to wait for nempty");
    ASSERT_ERR_QUIT(sem_wait(&shared.mutex) == 0, "Failed to wait for mutex");
    if (shared.nputval < nitems) {
      shared.buff[shared.nput++] = shared.nputval++;
      shared.nput %= NBUFF;
      *(int*)arg += 1;
      nextval = shared.nputval;
      ASSERT_ERR_QUIT(sem_post(&shared.mutex) == 0, "Failed to post to mutex");
      ASSERT_ERR_QUIT(sem_post(&shared.nstored) == 0, "Failed to post to nstored");
    } else {
      nextval = shared.nputval;
      ASSERT_ERR_QUIT(sem_post(&shared.nstored) == 0, "Failed to post to nstored");      
      ASSERT_ERR_QUIT(sem_post(&shared.nempty) == 0, "Failed to post to nempty");
      ASSERT_ERR_QUIT(sem_post(&shared.mutex) == 0, "Failed to post to mutex");
    }
  }
  printf("producer exit: %ld\n", pthread_self());
  return NULL;
}

void * consumer(void * arg) {
  int nextval = 0;
  *(int*)arg = 0;
  while (nextval < nitems) {
    ASSERT_ERR_QUIT(sem_wait(&shared.nstored) == 0, "Failed to wait for nstored");
    ASSERT_ERR_QUIT(sem_wait(&shared.mutex) == 0, "Failed to wait for mutex");
    if (shared.ngetval < nitems) {
      ASSERT_ERR_SYS(shared.buff[shared.nget] == shared.ngetval, "buff[%d] = %d, should be %d",
        shared.nget, shared.buff[shared.nget], shared.ngetval);
      shared.nget = (shared.nget + 1) % NBUFF;
      shared.ngetval++;
      *(int*) arg += 1;
      nextval = shared.ngetval; 
      ASSERT_ERR_QUIT(sem_post(&shared.mutex) == 0, "Failed to post to mutex");
      ASSERT_ERR_QUIT(sem_post(&shared.nempty) == 0, "Failed to post to nempty");
    } else {
      nextval = shared.ngetval;
      ASSERT_ERR_QUIT(sem_post(&shared.nstored) == 0, "Failed to post to nstored");
      ASSERT_ERR_QUIT(sem_post(&shared.mutex) == 0, "Failed to post to mutex");
    }
  }

  printf("consumer exit: %ld\n", pthread_self());
  return NULL;
}

