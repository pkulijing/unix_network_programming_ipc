#include "mqutils.h"
#include <pthread.h>
typedef struct tagmydata_t {
    mqd_t id;
    struct mq_attr attr;
    struct sigevent sigev;
} mydata_t;
static void notify_thread(union sigval arg) {
    mydata_t *data = arg.sival_ptr;
    void *buf = calloc(data->attr.mq_msgsize, 1);

    printf("notify_thread started: %ld.\n", (long)pthread_self());

    mq_notify(data->id, &data->sigev);

    ssize_t n;
    while ((n = mq_receive(data->id, buf, data->attr.mq_msgsize, NULL)) >= 0) {
        printf("read %ld bytes, msg:%s\n", 
                (long)n, (char*)buf);
        memset(buf, 0, data->attr.mq_msgsize);
    }
    ASSERT_ERR_SYS(errno == EAGAIN, "mq_receive error: errno = %d, %s",
        errno, strerror(errno));
    free(buf);
    pthread_exit(NULL);
}
int main(int argc, char** argv) {
    ASSERT_ERR_QUIT(argc == 2, "usage: mqnotifythread1 <name>");
    mydata_t *data = (mydata_t*)calloc(1, sizeof(mydata_t));
    
    data->id = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    ASSERT_ERR_QUIT(data->id >= 0, "Failed to open %s, errno = %d, %s",
        argv[1], errno, strerror(errno));

    int ret = mq_getattr(data->id, &data->attr);
    ASSERT_ERR_QUIT(ret == 0, "Failed to get attr of %s, errno = %d, %s",
        argv[1], errno, strerror(errno));

    data->sigev.sigev_notify = SIGEV_THREAD;
    data->sigev.sigev_value.sival_ptr = data;
    data->sigev.sigev_notify_function = notify_thread;
    data->sigev.sigev_notify_attributes = NULL;

    ASSERT_ERR_QUIT(mq_notify(data->id, &data->sigev) == 0, 
        "mq_notify failed,  errno = %d, %s", errno, strerror(errno));
    
    for (;;) {
        pause();
    }

    exit(0);
}