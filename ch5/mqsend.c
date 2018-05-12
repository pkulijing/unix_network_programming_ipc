#include "mqutils.h"
int main(int argc, char** argv) {
    if (argc != 4) {
        err_quit("usage: mqsend <name> <priority> <content>\n");
    }

    mqd_t id = mq_open(argv[1], O_WRONLY);
    if (id < 0) {
        err_quit("Failed to open mqueue: %s\n", argv[1]);
    }

    int len = strlen(argv[3]);
    int priority = strtol(argv[2], NULL, 0);
    int ret = mq_send(id, argv[3], len, priority);

    if (ret != 0) {
        err_quit("Failed to send, ret = %d\n", ret);
    } else {
        printf("Send success to %s, prio = %d, content = %s, len = %d",
            argv[1], priority, argv[3], len);
    }
    exit(0);
}