#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h> // mode_t
#include <sys/stat.h> // S_IRUSR

extern const int MAXLINE;
extern const char fifo_server[];
extern const char fifo_child_prefix[];
extern const mode_t FILE_MODE;

void fillChildFifoName(char* buf, size_t maxlen, long pid);
