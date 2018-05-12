#include "mqutils.h"
#include <sys/select.h>
int pipefd[2];
static void sig_usr1(int signo) {
    ssize_t n = write(pipefd[1], "", 1);
    ASSERT_ERR_QUIT(n == 1, "write failed.");
    return;
}
int main(int argc, char** argv) {
    ASSERT_ERR_QUIT(argc == 2, "usage: mqnotifysig2 <name>");

    mqd_t id = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    ASSERT_ERR_QUIT(id >= 0, "Failed to open %s, errno = %d, %s",
            argv[1], errno, strerror(errno));

    struct mq_attr attr;
    int ret = mq_getattr(id, &attr);
    ASSERT_ERR_QUIT(ret == 0, "Failed to get attr of %s, errno = %d, %s",
            argv[1], errno, strerror(errno));

    void* buf = calloc(attr.mq_msgsize, 1);
    ASSERT_ERR_QUIT(buf != NULL, "Failed to allocate memory for msg.");

    ASSERT_ERR_QUIT(pipe(pipefd) == 0, "pipe failed");

    __sighandler_t sig_old = signal(SIGUSR1, sig_usr1);
    ASSERT_ERR_QUIT(sig_old != SIG_ERR, 
        "Failed to specify signal handler for SIGUSR1, errno = %d, %s",
        errno, strerror(errno));
    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;

    ret = mq_notify(id, &sigev);
    ASSERT_ERR_QUIT(ret == 0, "mq_notify failed: errno = %d, %s",
        errno, strerror(errno));

    fd_set s;
    FD_ZERO(&s);

    for (;;) {
        FD_SET(pipefd[0], &s);
        int nfds = select(pipefd[0] + 1, &s, NULL, NULL, NULL);
        if (FD_ISSET(pipefd[0], &s)) {
            char c;
            ssize_t n = read(pipefd[0], &c, 1);
            ASSERT_ERR_QUIT(n == 1, "read failed");

            ASSERT_ERR_QUIT(mq_notify(id, &sigev) == 0, "mq_notify failed: errno = %d, %s",
                errno, strerror(errno));
            while ((n = mq_receive(id, buf, attr.mq_msgsize, NULL)) >= 0) {
                printf("SIGUSR1 received, read %ld bytes, msg:%s\n", 
                    (long)n, (char*)buf);
                memset(buf, 0, attr.mq_msgsize);
            }
            ASSERT_ERR_SYS(errno == EAGAIN, "mq_receive error: errno = %d, %s",
                errno, strerror(errno));
        }
    }

    mq_close(id);

    exit(0);
}
