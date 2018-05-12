#include "mqutils.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: mqattr <name>\n");
        exit(-1);
    }
    
    mqd_t id = mq_open(argv[1], O_RDONLY);
    if (id < 0) {
        printf("error: mq id = %d, errno = %d, %s\n", id, errno, strerror(errno));
        exit(-1);
    }

    struct mq_attr attr;
    int ret = mq_getattr(id, &attr);
    if (ret < 0) {
        printf("error: mq id = %d, errno = %d, %s\n", id, errno, strerror(errno));
        exit(-1);    
    }

    printf("max #msgs = %ld, max msg size = %ld, #cur msgs = %ld",
        attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
    
    mq_close(id);
    exit(0);
}