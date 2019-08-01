# Shared Memory

## Introduction

* Fastest form of IPC 
* No kernel involvement in passing data once memory is mapped
* Requires some form of synchronization
* When copying a file, data is copied fewer times when shared memory is used, in comparison with FIFO, pipe, or MQ
* Memory is not shared by default between a parent and a child

## `mmap`, `munmap`, `msync`

`mmap` maps a file or a Posix shared memory object into the address space of a process. Purposes:

* use with a regular file to provided memory-mapped I/O
* use special files to provide anonymous memory mappings
* use with `shm_open` to provide Posix shared memory between unrelated processes 

``` c
#include <sys/mman.h>
// returns starting address of mapped region if OK, MAP_FAILURE on error
void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);

```