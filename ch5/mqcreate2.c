#include "mqutils.h"
int main(int argc, char** argv) {
    int flags = O_RDWR | O_CREAT;
    int opt;
    struct mq_attr attr = {0};
    while ((opt = getopt(argc, argv, "em:z:")) != -1) {
        switch (opt) {
            case 'e':
                flags |= O_EXCL;
                break;
            case 'm':
                attr.mq_maxmsg = strtol(optarg, NULL, 0);
                break;
            case 'z':
                attr.mq_msgsize = strtol(optarg, NULL, 0);
                break;
            default:
                exit(-1);
                break;
        }
    }

    if (optind != argc - 1) {
        printf("usage: mqcreate2 [-e][-m maxmsg -z msgsize] <name>\n");
        exit(-1);
    }

    if (attr.mq_maxmsg != 0 && attr.mq_msgsize == 0 ||
        attr.mq_maxmsg == 0 && attr.mq_msgsize != 0) {
        printf("usage: mqcreate2 [-e][-m maxmsg -z msgsize] <name>\n");
        exit(-1);
    }

    /*On ubuntu: file created inside /dev/mqueue*/
    mqd_t mqd = mq_open(argv[optind], flags, FILE_MODE, 
        (attr.mq_maxmsg != 0) ? &attr : NULL);
    printf("queue name: %s, id: %ld, error = %d, %s\n", 
        argv[optind], (long)mqd, errno, strerror(errno));
    mq_close(mqd);

    exit(0);
}
