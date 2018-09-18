#include "utils.h"

const int MAXLINE = 1024;
const mode_t FILE_MODE = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
const char fifo_server[] = "/tmp/fifo.serv";
const char fifo_child_prefix[] = "/tmp/fifo.";

void fillChildFifoName(char* buf, size_t maxlen, long pid) {
    snprintf(buf, maxlen, "%s%ld", fifo_child_prefix, pid);
}

void err_sys(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}

void err_quit(const char* format, ...) {
    va_list args;
    va_start(args, format);
    err_sys(format, args);
    va_end(args);
    exit(-1);
}
