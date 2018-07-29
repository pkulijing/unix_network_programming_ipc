#ifndef MQ_UTILS_H
#define MQ_UTILS_H

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h> // mode_t
#include <sys/stat.h> // S_IRUSR
#include <mqueue.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>

extern const int MAXLINE;
extern const mode_t FILE_MODE;

void err_sys(const char* format, ...);
void err_quit(const char* format, ...);

#define ASSERT_ERR_QUIT(pred, format, ...) \
    if (!(pred)) { \
        err_quit(format, ##__VA_ARGS__); \
    }

#define ASSERT_ERR_SYS(pred, format, ...) \
    if (!(pred)) { \
        err_sys(format, ##__VA_ARGS__); \
    }

#endif //MQ_UTILS_H
