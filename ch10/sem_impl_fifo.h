
typedef struct {
  int sem_fd[2];
  int sem_magic;
} fifo_sem_t;

#define FIFO_SEM_FAILED ((fifo_sem_t*)(-1))
#define FIFO_SEM_MAGIC 0x89674523

fifo_sem_t *fifo_sem_open(const char* name, int oflag, ... /* mode_t mode, unsigned_int value */ );
int fifo_sem_close(fifo_sem_t *sem);
int fifo_sem_unlink(const char *name);
int fifo_sem_wait(fifo_sem_t *sem);
int fifo_sem_post(fifo_sem_t *sem);
