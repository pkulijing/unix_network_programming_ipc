#include "utils.h"

const mode_t FILE_MODE = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

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
