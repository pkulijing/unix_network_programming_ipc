#include <unistd.h>
#include "utils.h"

#define SEQFILE "seqno"

void my_lock(int fd);
void my_unlock(int fd);
int main(int argc, char** argv) {
    int i;
    int fd;
    ssize_t n;
    long seqno;
    char line[MAXLINE];

    fd = open(SEQFILE, O_RDWR, FILE_MODE);
    ASSERT_ERR_QUIT(fd > 0, "%s open failed. fd = %d", argv[0], fd);
    for (i = 0; i < 20; ++i) {
        my_lock(fd);
        lseek(fd, 0, SEEK_SET);
        n = read(fd, line, MAXLINE);
        ASSERT_ERR_QUIT(n > 0, "%s read failed. n = %d", argv[0], (int)n);
        line[n] = 0;
        sscanf(line, "%ld", &seqno);
        printf("%s with pid =  %ld, seqno = %ld\n", argv[0], (long)getpid(), seqno);
        seqno++;
        snprintf(line, sizeof(line), "%ld\n", seqno);
        lseek(fd, 0, SEEK_SET);
        write(fd, line, strlen(line));
        my_unlock(fd);
    }
    return 0;
}
