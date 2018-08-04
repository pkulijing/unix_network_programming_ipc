#ifndef UNPI_UTILS_H
#define UNPI_UTILS_H

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h> // mode_t
#include <sys/stat.h> // S_IRUSR
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>

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

#undef max
#undef min

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))

#endif //MQ_UTILS_H

