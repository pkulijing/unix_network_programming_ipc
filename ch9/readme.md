# Record Locking

## Introduction

* read-write lock (ch8): shared among threads in the same process or processes sharing a piece of shared memory
* record locking: share r/w of a file among related or unrelated processes.
    * performed by `fcntl`
    * file referenced through fd
    * owner of file identified by pid
    * maintained within the kernel
* granularity of record locking: size of the object that can be locked
    * the smaller the granularity, the greater the number of simutaneous users allowed
    * Posix record locking: ONE byte

