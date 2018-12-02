#include "utils.h"
#include "lock_reg.h"


int main(int argc, char** argv) {
  int pidfd;
  char line[MAXLINE];
  char PIDFILENAME[] = "pidfile";

  pidfd = open(PIDFILENAME, O_CREAT | O_RDWR, FILE_MODE);

  if (write_lock(pidfd, 0, SEEK_SET, 0) < 0) {
    if (errno == EAGAIN || errno == EACCES) {
      err_quit("unable to lock %s, %s is already running", PIDFILENAME, argv[0]);
    } else {
      err_quit("unable to lock %s for unknown reason. errno = %d", PIDFILENAME, errno);
    }
  }

  snprintf(line, sizeof(line), "%ld\n", (long)getpid());

  ftruncate(pidfd, 0);

  write(pidfd, line, strlen(line));

  pause();

  exit(0);
}