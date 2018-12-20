#include "utils.h"
#include "sem_impl_fifo.h"

/* mode_t mode, unsigned_int value */
fifo_sem_t *fifo_sem_open(const char* name, int oflag, ...) {
  fifo_sem_t *sem;
  va_list ap;
  mode_t mode;
  unsigned int value;
  char c;
  int i, save_errno,flags;

  if (oflag & O_CREAT) {
    va_start(ap, oflag);
    mode = va_arg(ap, mode_t);
    value = va_arg(ap, unsigned int);
    va_end(ap);
    if (mkfifo(name, mode) < 0) {
      if (errno == EEXIST && (oflag & O_EXCL) == 0) {
        oflag &= ~O_CREAT;
      } else {
        return FIFO_SEM_FAILED;
      }
    }
  }

  sem = (fifo_sem_t*)malloc(sizeof(fifo_sem_t));
  if (sem == NULL) {
    return FIFO_SEM_FAILED;
  }

  //NONBLOCK to avoid blocking here
  sem->sem_fd[0] = open(name, O_RDONLY | O_NONBLOCK);
  sem->sem_fd[1] = open(name, O_WRONLY | O_NONBLOCK);

  if (sem->sem_fd[0] < 0 || sem->sem_fd[1] < 0) {
    goto fifo_sem_open_error;
  }

  // reset NONBLOCK flag
  flags = fcntl(sem->sem_fd[0], F_GETFL);
  if (flags < 0) {
    goto fifo_sem_open_error;
  }

  flags &= ~O_NONBLOCK;

  if (fcntl(sem->sem_fd[0], F_SETFL, flags) < 0) {
    goto fifo_sem_open_error;
  }

  if (oflag & O_CREAT) {
    for (i = 0; i < value; ++i) {
      if (write(sem->sem_fd[1], &c, 1) != 1) {
        goto fifo_sem_open_error;
      }
    }
  }

  sem->sem_magic = FIFO_SEM_MAGIC;
  return sem;

fifo_sem_open_error:
  save_errno = errno;
  if (oflag & O_CREAT) {
    unlink(name);
  }
  close(sem->sem_fd[0]);
  close(sem->sem_fd[1]);
  free(sem);
  errno = save_errno;
  return FIFO_SEM_FAILED;
}
int fifo_sem_close(fifo_sem_t *sem) {
  if (sem->sem_magic != FIFO_SEM_MAGIC) {
    errno = EINVAL;
    return -1;
  }
  sem->sem_magic = 0;
  if (close(sem->sem_fd[0]) == -1 || close(sem->sem_fd[1]) == -1) {
    free(sem);
    return -1;
  }
  free(sem);
  return 0;
}
int fifo_sem_unlink(const char *name) {
  return unlink(name);
}
int fifo_sem_wait(fifo_sem_t *sem) {
  char c;

  if (sem->sem_magic != FIFO_SEM_MAGIC) {
    errno = EINVAL;
    return -1;
  }

  if (read(sem->sem_fd[0], &c, 1) != 1) {
    return -1;
  }
  return 0;
}
int fifo_sem_post(fifo_sem_t *sem) {
  char c;

  if (sem->sem_magic != FIFO_SEM_MAGIC) {
    errno = EINVAL;
    return -1;
  }

  if (write(sem->sem_fd[1], &c, 1) != 1) {
    return -1;
  }
  return 0;
}