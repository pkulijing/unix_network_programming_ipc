#include "utils.h"
volatile sig_atomic_t mqflag;
static void sig_usr1(int signo) {
    mqflag = 1;
    return;
}
int main(int argc, char** argv) {
    ASSERT_ERR_QUIT(argc == 2, "usage: mqnotifysig2 <name>");

    mqd_t id = mq_open(argv[1], O_RDONLY);
    ASSERT_ERR_QUIT(id >= 0, "Failed to open %s, errno = %d, %s",
            argv[1], errno, strerror(errno));

    struct mq_attr attr;
    int ret = mq_getattr(id, &attr);
    ASSERT_ERR_QUIT(ret == 0, "Failed to get attr of %s, errno = %d, %s",
            argv[1], errno, strerror(errno));

    void* buf = calloc(attr.mq_msgsize, 1);
    ASSERT_ERR_QUIT(buf != NULL, "Failed to allocate memory for msg.");

    sigset_t zeromask, oldmask, newmask;

    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigemptyset(&oldmask);
    sigaddset(&newmask, SIGUSR1);

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

    for (;;) {
        sigprocmask(SIG_BLOCK, &newmask, &oldmask);
        while (mqflag == 0) {
            sigsuspend(&zeromask);
        }
        mqflag = 0;
        ASSERT_ERR_QUIT(mq_notify(id, &sigev) == 0, "mq_notify failed: errno = %d, %s",
            errno, strerror(errno));
        memset(buf, 0, attr.mq_msgsize);
        sleep(10);
        ssize_t n = mq_receive(id, buf, attr.mq_msgsize, NULL);
        printf("SIGUSR1 received, read %ld bytes, msg:%s\n", 
            (long)n, (char*)buf);
        sigprocmask(SIG_UNBLOCK, &newmask, NULL);
    }

    mq_close(id);

    exit(0);
}
