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

#define MAX_MSG_DATA (2048 - 2 * sizeof(long))

typedef struct mymsg {
    long msg_len;
    long msg_type;
    char msg_data[MAX_MSG_DATA];
} MyMsg;

#define MSG_HEADER_SIZE (sizeof(struct mymsg) - MAX_MSG_DATA)

ssize_t sendMsg(int fd, MyMsg* msg) {
    return write(fd, msg, msg->msg_len + MSG_HEADER_SIZE);
}

ssize_t receiveMsg(int fd, MyMsg* msg) {
    ssize_t n = read(fd, msg, MSG_HEADER_SIZE);
    if (n == 0) {
        return 0;
    } else if (n != MSG_HEADER_SIZE) {
        exit(-1);
    }

    if (msg->msg_len > 0) {
        n = read(fd, msg->msg_data, msg->msg_len);
        if (n != msg->msg_len) {
            exit(-2);
        }
    }
    return msg->msg_len;
}


void client(int fd_r, int fd_w) {
    printf("parent (client)\n");
    MyMsg msg;

    // read file name
    fgets(msg.msg_data, MAX_MSG_DATA, stdin);
    
    // write file name to pipe
    int len = strlen(msg.msg_data);
    if (msg.msg_data[len - 1] == '\n') {
        --len;
    }
    
    msg.msg_len = len;
    msg.msg_type = 1;

    sendMsg(fd_w, &msg);

    // read file content / error msg from pipe
    while ((len = receiveMsg(fd_r, &msg)) > 0) {
        write(STDOUT_FILENO, msg.msg_data, len);
    }
}

void server(int fd_r, int fd_w) {
    printf("child (server)\n");

    // read file name from pipe
    MyMsg msg;
    ssize_t len = receiveMsg(fd_r, &msg);
    msg.msg_data[len] = '\0';

    int fd = open(msg.msg_data, O_RDONLY, 0);

    if (fd < 0) {
        snprintf(msg.msg_data + len, MAX_MSG_DATA - len, ": failed to open, %s\n", strerror(errno));
        msg.msg_len = strlen(msg.msg_data);
        sendMsg(fd_w, &msg);
    } else {
        while ((len = read(fd, msg.msg_data, MAX_MSG_DATA)) > 0) {
            msg.msg_len = len;
            sendMsg(fd_w, &msg);
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