#include "mqutils.h"
int main(int argc, char** argv) {
    int flags = O_RDWR | O_CREAT;
    int opt;
    while ((opt = getopt(argc, argv, "e")) != -1) {
        switch (opt) {
            case 'e':
                flags |= O_EXCL;
                break;
            default:
                exit(-1);
                break;
        }
    }

    if (optind != argc - 1) {
        printf("usage: mqcreate1 [-e] <name>\n");
        exit(-1);
    }

    /*On ubuntu: file created inside /dev/mqueue*/
    mqd_t mqd = mq_open(argv[optind], flags, FILE_MODE, NULL);
    printf("queue name: %s, id: %ld, error = %d, %s\n", 
        argv[optind], (long)mqd, errno, strerror(errno));
    mq_close(mqd);

    exit(0);
}
