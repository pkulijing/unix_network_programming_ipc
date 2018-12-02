#include <fcntl.h>
#include <unistd.h>
#include "utils.h"
#include "lock_reg.h"

int main(int argc, char** argv) {
  int fd = open("testfile", O_RDWR | O_CREAT, FILE_MODE);

  printf("%s: parent: I'll try to get the write lock\n", current_time());
  printf("parent writew_lock: %d\n", writew_lock(fd, 0, SEEK_SET, 0));
  printf("%s: parent: I got the write lock\n", current_time());

  if (fork() == 0) {
    sleep(3);
    printf("%s: child 1: I'll try to obtain a write lock\n", current_time());
    printf("child 1 writew_lock: %d\n", writew_lock(fd, 0, SEEK_SET, 0));
    printf("%s: child 1: I got the write lock\n", current_time());
    sleep(2);
    un_lock(fd, 0, SEEK_SET, 0);
    printf("%s: child 1: I released the write lock\n", current_time());
    exit(0);
  }

  if (fork() == 0) {
    sleep(1);
    printf("%s: child 2: I'll try to obtain a read lock\n", current_time());
    printf("child 2 readw_lock: %d\n", readw_lock(fd, 0, SEEK_SET, 0));
    printf("%s: child 2: I got the read lock\n", current_time());
    sleep(4);
    un_lock(fd, 0, SEEK_SET, 0);
    printf("%s: child 2: I released the read lock\n", current_time());
    exit(0);
  }

  sleep(5);
  un_lock(fd, 0, SEEK_SET, 0);
  printf("%s: parent: I released the read lock\n", current_time());
  exit(0);
}