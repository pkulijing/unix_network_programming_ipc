#include <fcntl.h>
#include <unistd.h>
#include "utils.h"
void my_lock(int fd) {
  int ret;
  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  ret = fcntl(fd, F_SETLKW, &lock);
  ASSERT_ERR_QUIT(ret == 0, "failed to obtain write lock of fd: ", fd);
}
void my_unlock(int fd) {
  int ret;
  struct flock lock;
  lock.l_type = F_UNLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  ret = fcntl(fd, F_SETLK, &lock);
  ASSERT_ERR_QUIT(ret == 0, "failed to release lock of fd: ", fd);
}