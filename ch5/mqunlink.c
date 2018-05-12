#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: mqunlink <name>\n");
    }

    int ret = mq_unlink(argv[1]);
    printf("Unlink %s returns %d, errno = %d, %s\n",
        argv[1], ret, errno, strerror(errno));
}