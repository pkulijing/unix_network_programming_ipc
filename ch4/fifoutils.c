#include "fifoutils.h"

const int MAXLINE = 1024;
const char fifo_server[] = "/tmp/fifo.serv";
const char fifo_child_prefix[] = "/tmp/fifo.";
const mode_t FILE_MODE = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

void fillChildFifoName(char* buf, size_t maxlen, long pid) {
    snprintf(buf, maxlen, "%s%ld", fifo_child_prefix, pid);
}
