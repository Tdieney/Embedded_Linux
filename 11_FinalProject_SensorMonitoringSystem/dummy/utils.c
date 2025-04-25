#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_event(const char *fifo_name, const char *message) {
    pthread_mutex_lock(&log_mutex);
    int fd = open(fifo_name, O_WRONLY | O_NONBLOCK);
    if (fd >= 0) {
        write(fd, message, strlen(message) + 1);
        close(fd);
    }
    pthread_mutex_unlock(&log_mutex);
}
