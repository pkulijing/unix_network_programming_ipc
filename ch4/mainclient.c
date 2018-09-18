#include <unistd.h> // pipe, fork, close, STDOUT_FILENO
#include <sys/wait.h> // waitpid
#include <sys/types.h> // pid_t
#include <sys/stat.h>
#include <stdlib.h> // exit
#include <fcntl.h>
#include <stdio.h>
#include <string.h> // strerrno
#include <errno.h> // errno
#include <assert.h>
#include <unistd.h>

#include "utils.h"

int main(int argc, char** argv) {

    char fifo_from_server[MAXLINE];
    fillChildFifoName(fifo_from_server, MAXLINE, getpid());

    if (mkfifo(fifo_from_server, FILE_MODE) < 0 && errno != EEXIST) {
        printf("%s: Failed to make fifo_from_server\n", argv[0]);
        exit(-1);
    }
    
    char request[MAXLINE], buf[MAXLINE];
    fgets(buf, MAXLINE, stdin);
    snprintf(request, MAXLINE, "%d %s", getpid(), buf);
    
    int fd_w = open(fifo_server, O_WRONLY, 0);
    if (fd_w < 0) {
        printf("%s: Failed to open fifo_server\n", argv[0]);
        unlink(fifo_server);
        exit(-2);
    }
    
    write(fd_w, request, MAXLINE);

    int fd_r = open(fifo_from_server, O_RDONLY, 0);    
    if (fd_r < 0) {
        printf("%s: Failed to open fifo_from_server\n", argv[0]);
        unlink(fifo_server);
        unlink(fifo_from_server);
        exit(-2);
    }
    int len;
    while((len = read(fd_r, buf, MAXLINE)) > 0) {
        write(STDOUT_FILENO, buf, len);
    }
    close(fd_r);
    unlink(fifo_from_server);
    exit(0);
}
