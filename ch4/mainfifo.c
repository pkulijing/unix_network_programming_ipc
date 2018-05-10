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

const int READ = 0;
const int WRITE = 1;
const int MAXLINE = 1024;

void client(int fd_r, int fd_w) {
    printf("parent (client)\n");

    // read file name
    char buf[MAXLINE];
    fgets(buf, MAXLINE, stdin);
    
    // write file name to pipe
    int len = strlen(buf);
    if (buf[len - 1] == '\n') {
        --len;
    }
    write(fd_w, buf, len);

    // read file content / error msg from pipe
    while ((len = read(fd_r, buf, MAXLINE)) > 0) {
        write(STDOUT_FILENO, buf, len);
    }
}
void server(int fd_r, int fd_w) {
    printf("child (server)\n");

    // read file name from pipe
    char buf[MAXLINE];
    int len = read(fd_r, buf, MAXLINE);
    assert(len > 0);

    int fd = open(buf, O_RDONLY);

    if (fd < 0) {
        snprintf(buf + len, MAXLINE - len, ": failed to open, %s\n", strerror(errno));
        write(fd_w, buf, strlen(buf));
    } else {
        while ((len = read(fd, buf, MAXLINE)) > 0) {
            write(fd_w, buf, len);
        }
        close(fd);
    }
}

const char fifo_name_c2p[] = "/tmp/child2parent";
const char fifo_name_p2c[] = "/tmp/parent2child";
const mode_t FILE_MODE = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

int main(int argc, char** argv) {

    int fd_r, fd_w;
    
    if (mkfifo(fifo_name_c2p, FILE_MODE) < 0 && errno != EEXIST)  {
        exit(-1);
    }

    if (mkfifo(fifo_name_p2c, FILE_MODE) < 0 && errno != EEXIST) {
        unlink(fifo_name_c2p);
        exit(-1);
    }

    pid_t child_pid = fork();

    if (child_pid == 0) {
        fd_r = open(fifo_name_p2c, O_RDONLY, 0);
        fd_w = open(fifo_name_c2p, O_WRONLY, 0);

        server(fd_r, fd_w);

        exit(0);
    }

    fd_w = open(fifo_name_p2c, O_WRONLY, 0);
    fd_r = open(fifo_name_c2p, O_RDONLY, 0);

    client(fd_r, fd_w);

    waitpid(child_pid, NULL, 0);

    close(fd_r);
    close(fd_w);

    unlink(fifo_name_c2p);
    unlink(fifo_name_p2c);

    exit(0);
}