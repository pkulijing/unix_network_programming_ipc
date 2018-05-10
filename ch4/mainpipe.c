#include <unistd.h> // pipe, fork, close, STDOUT_FILENO
#include <sys/wait.h> // waitpid
#include <sys/types.h> // pid_t
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

int main(int argc, char** argv) {
    int child2parent[2], parent2child[2];

    pid_t child_pid;

    pipe(child2parent);
    pipe(parent2child);

    printf("pipe1: read %d, write %d, pipe2: read %d, write %d\n", child2parent[0], child2parent[1], parent2child[0], parent2child[1]);

    child_pid = fork();

    if (child_pid == 0) { 
        //child (server)
        close(child2parent[READ]);
        close(parent2child[WRITE]);

        server(parent2child[READ], child2parent[WRITE]);
        printf("child (server) exits\n");
        exit(0);
    }

    //parent (client)

    close(parent2child[READ]);
    close(child2parent[WRITE]);

    client(child2parent[READ], parent2child[WRITE]);

    waitpid(child_pid, NULL, 0);
    printf("parent (client) exits\n");
    exit(0);
}