#include "lock_reg.h"

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len) {
  struct flock lock;
  lock.l_type = type;
  lock.l_start = offset;
  lock.l_len = len;
  lock.l_whence = whence;
  return fcntl(fd, cmd, &lock);
}

pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len) {
  struct flock lock;
  lock.l_type = type;
  lock.l_start = offset;
  lock.l_len = len;
  lock.l_whence = whence;

  if (fcntl(fd, F_GETLK, &lock) == -1) {
    return -1;
  }

  if (lock.l_pid == F_UNLCK) {
    return 0;
  }

  return lock.l_pid;
}
