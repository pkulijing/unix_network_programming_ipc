#include "mqutils.h"
mqd_t id;
void *buf;
struct mq_attr attr;
struct sigevent sigev;
static void sig_usr1(int signo) {
    int ret = mq_notify(id, &sigev);
    ASSERT_ERR_QUIT(ret == 0, "mq_notify failed: errno = %d, %s",
        errno, strerror(errno));
    int n = mq_receive(id, buf, attr.mq_msgsize, NULL);
    printf("SIGUSR1 received, read %ld bytes, msg:%s\n", (long)n, (char*)buf);
    return;
}
int main(int argc, char** argv) {
    ASSERT_ERR_QUIT(argc == 2, "usage: mqnotifysig1 <name>");

    id = mq_open(argv[1], O_RDONLY);
    ASSERT_ERR_QUIT(id >= 0, "Failed to open %s, errno = %d, %s",
            argv[1], errno, strerror(errno));

    int ret = mq_getattr(id, &attr);
    ASSERT_ERR_QUIT(ret == 0, "Failed to get attr of %s, errno = %d, %s",
            argv[1], errno, strerror(errno));

    buf = calloc(attr.mq_msgsize, 1);
    ASSERT_ERR_QUIT(buf != NULL, "Failed to allocate memory for msg.");

    __sighandler_t sig_old = signal(SIGUSR1, sig_usr1);
    ASSERT_ERR_QUIT(sig_old != SIG_ERR, 
        "Failed to specify signal handler for SIGUSR1, errno = %d, %s",
        errno, strerror(errno));
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;

    ret = mq_notify(id, &sigev);
    ASSERT_ERR_QUIT(ret == 0, "mq_notify failed: errno = %d, %s",
        errno, strerror(errno));

    for (;;)
        pause();

    mq_close(id);

    exit(0);
}
