#include "utils.h"
volatile sig_atomic_t mqflag;
static void sig_usr1(int signo) {
    mqflag = 1;
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

    sigset_t newmask;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &newmask, NULL);

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

    int signo;
    for (;;) {
        ASSERT_ERR_QUIT(sigwait(&newmask, &signo) == 0, "sigwait error");
        if (signo == SIGUSR1) {
            mqflag = 0;
            ASSERT_ERR_QUIT(mq_notify(id, &sigev) == 0, "mq_notify failed: errno = %d, %s",
                errno, strerror(errno));
            ssize_t n;
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
