#ifndef UNPI_UTILS_H
#define UNPI_UTILS_H

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h> // mode_t
#include <sys/stat.h> // S_IRUSR
#if __linux__
#include <mqueue.h>
#endif
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <stdint.h>
extern const int MAXLINE;
extern const mode_t FILE_MODE;
extern const char fifo_server[];
extern const char fifo_child_prefix[];

void err_sys(const char* format, ...);
void err_quit(const char* format, ...);
void fillChildFifoName(char* buf, size_t maxlen, long pid);
const char* current_time();

#define ASSERT_ERR_QUIT(pred, format, ...) \
    if (!(pred)) { \
        fprintf(stderr, "[%s:%d]", __FILE__, __LINE__); \
        err_quit(format, ##__VA_ARGS__); \
    }

#define ASSERT_ERR_SYS(pred, format, ...) \
    if (!(pred)) { \
        fprintf(stderr, "[%s:%d]", __FILE__, __LINE__); \
        err_sys(format, ##__VA_ARGS__); \
    }

#undef max
#undef min

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))

#endif //MQ_UTILS_H

