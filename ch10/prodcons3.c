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

struct {
  int buff[NBUFF];
  int nput;
  int nputval;
  sem_t mutex, nempty, nstored;
} shared;

int main(int argc, char** argv) {
  pthread_t tid_producer[MAXTHREADS], tid_consumer;
  int i, count[MAXTHREADS];

  ASSERT_ERR_QUIT(argc == 3, "usage: %s [number of items] [number of producers]", argv[0]);

  nitems = atoi(argv[1]);
  nproducers = atoi(argv[2]);

  ASSERT_ERR_QUIT(sem_init(&shared.mutex, 0, 1) != -1, "Failed to init mutex: %d", errno);
  ASSERT_ERR_QUIT(sem_init(&shared.nempty, 0, NBUFF) != -1, "Failed to init nempty: %d", errno);
  ASSERT_ERR_QUIT(sem_init(&shared.nstored, 0, 0) != -1, "Failed to init nstored: %d", errno);

  shared.nput = 0;
  shared.nputval = 0;

  for (i = 0; i < nproducers; ++i) {
    ASSERT_ERR_QUIT(pthread_create(&tid_producer[i], NULL, &producer, &count[i]) == 0, 
      "Failed to create producer thread");
  }

  ASSERT_ERR_QUIT(pthread_create(&tid_consumer, NULL, &consumer, NULL) == 0,
    "Failed to create consumer thread");

  int total = 0;

  for (i = 0; i < nproducers; ++i) {
    ASSERT_ERR_QUIT(pthread_join(tid_producer[i], NULL) == 0, 
      "Failed to join producer thread: %ld", tid_producer[i]);
    total += count[i];
    printf("count[%d] = %d\n", i, count[i]);
  }
  ASSERT_ERR_QUIT(pthread_join(tid_consumer, NULL) == 0, 
    "Failed to join consumer thread: %ld", tid_consumer);

  ASSERT_ERR_QUIT(0 == sem_destroy(&shared.mutex), "Failed to destroy semaphore mutex");
  ASSERT_ERR_QUIT(0 == sem_destroy(&shared.nempty), "Failed to destroy semaphore nempty");
  ASSERT_ERR_QUIT(0 == sem_destroy(&shared.nstored), "Failed to destroy semaphore nstored");

  ASSERT_ERR_QUIT(total == nitems, "total = %d, nitems = %d", total, nitems);

  return 0;
}

void * producer(void * arg) {
  int i, nextval = 0;

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
      ASSERT_ERR_QUIT(sem_post(&shared.nempty) == 0, "Failed to post to nempty");
      ASSERT_ERR_QUIT(sem_post(&shared.mutex) == 0, "Failed to post to mutex");
    }
  }
  printf("producer exit: %ld\n", pthread_self());
  return NULL;
}

void * consumer(void * arg) {
  int i;
  for (i = 0; i < nitems; ++i) {
    ASSERT_ERR_QUIT(sem_wait(&shared.nstored) == 0, "Failed to wait for nstored");
    ASSERT_ERR_QUIT(sem_wait(&shared.mutex) == 0, "Failed to wait for mutex");
    ASSERT_ERR_SYS(shared.buff[i % NBUFF] == i, "buff[%d] = %d, should be %d",
      i % NBUFF, shared.buff[i % NBUFF], i);
    ASSERT_ERR_QUIT(sem_post(&shared.mutex) == 0, "Failed to post to mutex");
    ASSERT_ERR_QUIT(sem_post(&shared.nempty) == 0, "Failed to post to nempty");
  }

  printf("consumer exit: %ld\n", pthread_self());
  return NULL;
}

