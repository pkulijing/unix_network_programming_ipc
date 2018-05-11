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

#include "fifoutils.h"

int main(int argc, char** argv) {
    if (mkfifo(fifo_server, FILE_MODE) < 0 && errno != EEXIST) {
        printf("Failed to create the server fifo: %s\n", fifo_server);
        exit(-1);
    }

    int fd_r = open(fifo_server, O_RDONLY, 0);
    //! ref: unpv2 page 61
    int dummy_fd_w = open(fifo_server, O_WRONLY, 0);

    if (fd_r < 0) {
        printf("Failed to open the server fifo: %s\n", fifo_server);
        unlink(fifo_server);
        exit(-2);
    }

    char request[MAXLINE], fifo_tochild[MAXLINE], buf[MAXLINE];
    
    while (read(fd_r, request, MAXLINE) > 0) {
        char *path = strchr(request, ' ');
        if (path == NULL) {
            printf("Bad request: %s\n", request);
            continue;
        }

        *path++ = 0;
        long child_pid = strtol(request, NULL, 0);
        fillChildFifoName(fifo_tochild, MAXLINE, child_pid);
        int len = strlen(path);
        if (path[len - 1] == '\n') {
            path[len - 1] = 0;
        }

        printf("Child pid: %ld, path: %s\n", child_pid, path);
        
        //! The fifo should have been opened by the child.
        int fd_w = open(fifo_tochild, O_WRONLY, 0);
        if (fd_w < 0) {
            printf("Failed to open the child fifo: %s\n", fifo_tochild);
            continue;
        }

        int fd_file = open(path, O_RDONLY, 0);
        
        if (fd_file < 0) {
            snprintf(buf, MAXLINE, "%s: failed to open, %s\n", path, strerror(errno));
            write(fd_w, buf, strlen(buf));
        } else {
            while ((len = read(fd_file, buf, MAXLINE)) > 0) {
                write(fd_w, buf, len);
            }
            close(fd_file);
        }
        close(fd_w);
    }
    exit(0);
}