#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include "utils.h"

void *producer(void *);
void *consumer(void *);

#define NBUFF 10
#define SEM_MUTEX "/sem_mutex"
#define SEM_NEMPTY "/sem_nempty"
#define SEM_NSTORED "/sem_nstored"

int nitems;

struct {
  int buff[NBUFF];
  sem_t *mutex, *nempty, *nstored;
} shared;

int main(int argc, char** argv) {
  pthread_t tid_producer, tid_consumer;

  ASSERT_ERR_QUIT(argc == 2, "usage: %s [number of items]", argv[0]);

  nitems = atoi(argv[1]);

  sem_unlink(SEM_MUTEX);
  sem_unlink(SEM_NEMPTY);  
  sem_unlink(SEM_NSTORED);  

  shared.mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, FILE_MODE, 1);
  shared.nempty = sem_open(SEM_NEMPTY, O_CREAT | O_EXCL, FILE_MODE, NBUFF);
  shared.nstored = sem_open(SEM_NSTORED, O_CREAT | O_EXCL, FILE_MODE, 0);

  ASSERT_ERR_QUIT(shared.mutex != SEM_FAILED, "Failed to create semaphore mutex: %d", errno);
  ASSERT_ERR_QUIT(shared.nempty != SEM_FAILED, "Failed to create semaphore nempty: %d", errno);
  ASSERT_ERR_QUIT(shared.nstored != SEM_FAILED, "Failed to create semaphore nstored: %d", errno);

  ASSERT_ERR_QUIT(pthread_create(&tid_producer, NULL, &producer, NULL) == 0, 
    "Failed to create producer thread");
  ASSERT_ERR_QUIT(pthread_create(&tid_consumer, NULL, &consumer, NULL) == 0,
    "Failed to create consumer thread");

  ASSERT_ERR_QUIT(pthread_join(tid_producer, NULL) == 0, 
    "Failed to join producer thread: %ld", tid_producer);
  ASSERT_ERR_QUIT(pthread_join(tid_consumer, NULL) == 0, 
    "Failed to join consumer thread: %ld", tid_consumer);
  
  ASSERT_ERR_QUIT(0 == sem_unlink(SEM_MUTEX), "Failed to unlink semaphore mutex");
  ASSERT_ERR_QUIT(0 == sem_unlink(SEM_NEMPTY), "Failed to unlink semaphore nempty");
  ASSERT_ERR_QUIT(0 == sem_unlink(SEM_NSTORED), "Failed to unlink semaphore nstored");

  return 0;
}

void * producer(void * arg) {
  int i;
  for (i = 0; i < nitems; ++i) {
    ASSERT_ERR_QUIT(sem_wait(shared.nempty) == 0, "Failed to wait for nempty");
    ASSERT_ERR_QUIT(sem_wait(shared.mutex) == 0, "Failed to wait for mutex");
    shared.buff[i % NBUFF] = i;
    ASSERT_ERR_QUIT(sem_post(shared.mutex) == 0, "Failed to post to mutex");
    ASSERT_ERR_QUIT(sem_post(shared.nstored) == 0, "Failed to post to nstored");
  }

  return NULL;
}

void * consumer(void * arg) {
  int i;
  for (i = 0; i < nitems; ++i) {
    ASSERT_ERR_QUIT(sem_wait(shared.nstored) == 0, "Failed to wait for nstored");
    ASSERT_ERR_QUIT(sem_wait(shared.mutex) == 0, "Failed to wait for mutex");
    ASSERT_ERR_SYS(shared.buff[i % NBUFF] == i, "buff[%d] = %d, should be %d",
      i % NBUFF, shared.buff[i % NBUFF], i);
    ASSERT_ERR_QUIT(sem_post(shared.mutex) == 0, "Failed to post to mutex");
    ASSERT_ERR_QUIT(sem_post(shared.nempty) == 0, "Failed to post to nempty");
  }

  return NULL;
}

