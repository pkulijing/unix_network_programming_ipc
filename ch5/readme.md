# Posix Message Queues

## Introduction

* A message queue can be thought of as a linked list of messages
* Different from pipes and fifos

    <table>
    <thead><tr>
        <th>Pipes &amp; Fifos</th>
        <th>Message Queues</th>
    </tr></thead>
    <tbody>
        <tr>
            <td>Process persistent: write without reader generates <strong>SIGPIPE</strong></td>
            <td>Kernel persistent: write message without reader is ok (messages can be read after writer terminates)</td>
        </tr>
        <tr>
            <td>Byte streams with no message boundaries and no types associated with each message</td>
            <td>Each message on a queue is a record with attributes:
            <ul>
                <li>an uint priority (Posix) or a long type (System V)</li>
                <li>the length of the data portion (can be 0)</li>
                <li>the data itself</li>
            </ul>
            </td>
        </tr>
    </tbody>
    </table>
* Difference between Posix and System V
    <table>
    <thead><tr>
        <th>Posix Message Queues</th>
        <th>System V Message Queues</th>
    </tr></thead>
    <tbody>
        <tr>
        <td>Read returns oldest message of highest priority</td>
        <td>Read can return a message of any desired priority</td>
        </tr>
        <tr>
        <td>Allow generating a signal or initiate a thread when a message is placed on an empty queue</td>
        <td>No similar mechanism</td>
        </tr>
    </tbody>
    </table>

## Programming Interface

```c
#include <mqueue.h>
// Create a new message queue or open an existing one.
// This is variable arguments, not function overloading. Check https://stackoverflow.com/questions/2685933/function-overloading-in-c
mqd_t mq_open(const char *name, int oflag);
mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr);

// Close an open message queue
// Calling process can no longer use the descriptor, but the message queue is not removed from the system.
int mq_close(mqd_t mqdes);

// Remove name used as an argument of mq_open from the system
// Message queues has a reference of how many times they are currently open. This function removes the name from the system, but the queue is not destroyed until the reference count reaches 0 (i.e. the last mq_close occurs).
int mq_unlink(const char *name);

// Get & set attributes
// 1. mq_maxmsg and mq_msgsize can only be set with mq_open
// 2. mq_setattr is used to set the O_NONBLOCK flag
// 3. mq_curmsgs can be fetched but not set
struct mq_attr {
    long mq_flags; /* flags: 0, O_NONBLOCK */
    long mq_maxmsg; /* max number of messages allowed on queue */
    long mq_msgsize; /* max size of a message (in bytes) */
    long mq_curmsgs; /* number of messages currently on the queue */
};
int mq_getattr(mqd_t mqdes, struct mq_attr *attr);
int mq_setattr(mqd_t mqdes, const struct mq_attr *attr, struct mq_attr *oattr);

// Send & received messages
// mq_receive always returns the oldest message of the highest priority. the len argument passed to mq_receive must >= mq_msgsize of the queue, otherwise it returns EMSGSIZE.
int mq_send(mqd_t mqdes, const char *ptr, size_t len, unsigned int prio);
ssize_t mq_receive(mqd_t mqdes, char* ptr, size_t len, unsigned int *priop);
```
## Message Queue Limits

There are two system-wide limits besides `mq_maxmsg` and `mq_msgsize`
* `MQ_OPEN_MAX`: max number of message queues that a process can have open at once (at least 8 as per Posix)
* `MQ_PRIO_MAX`: max value plus one for the priority of any message (at least 32 as per Posix)

## `mq_notify` Function

* Posix message queues allow for an asynchronous event notification when a message is placed onto an empty queue. It can be:
    - the generation of a signal
    - the creation of a thread to execute a specified function
* Lack of such mechanism in System V message queues causes *polling*, which is a waste of CPU time 

```c
#include <mqueue.h>
union sigval {
    int      sigval_int; /* Integer value */     
    void    *sigval_ptr; /* Pointer value */
};
struct sigevent {
    int sigev_notify; /* SIGEV_{NONE, SIGNAL, THREAD} */
    int sigev_signo; /* signal number if SIGEV_SIGNAL */
    union sigval sigev_value; /* passed to signal handler or thread */
                              /* following to if SIGEV_THREAD */
    void (*sigev_notify_function)(union sigval);
    pthread_attr_t *sigev_notify_attributes;
};
int mq_notify(mqd_t mqdes, const struct sigevent *notification);
```
* If *notification* is not null, then the process wants to be notified when an message arrives on the specified queue and the queue is empty (the process is registered for notification for the queue).
* If *notification* is null and if the process is currently registered for notification for the queue, the existing registration is removed.
* Only one process at any given time can be registered for notification for a given queue.
* Blocking in a call to `mq_receive` takes precedence over any registration for notification.
* When the notification is sent to the registered process, the registration is removed. The process must registered again (if desired).
* When using `SIGEV_SIGNAL`, take care to use only *async-signal-safe* functions (functions allowed to be used in a signal handler), or to avoid function calls in the signal handler using other mechanisms.
