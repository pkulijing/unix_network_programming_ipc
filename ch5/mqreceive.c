#include "mqutils.h"
int main(int argc, char** argv) {
    if (argc != 2) {
        err_quit("usage: mqreceive <name> <priority>\n");
    }

    mqd_t id = mq_open(argv[1], O_RDONLY);
    if (id < 0) {
        err_quit("Failed to open mqueue: %s\n", argv[1]);
    }

    struct mq_attr attr;
    int ret = mq_getattr(id, &attr);
    if (ret != 0) {
        err_quit("Failed to get attr of: %s\n", argv[1]);
    }   

    char* buf = calloc(attr.mq_msgsize, 1);

    int priority;
    int n = mq_receive(id, buf, attr.mq_msgsize, &priority);

    printf("Received %d bytes, priority = %d, content: %s\n",
        n, priority, buf);
}