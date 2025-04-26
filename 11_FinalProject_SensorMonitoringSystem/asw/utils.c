#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "common.h"

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Write a log message to a named FIFO with thread safety.
 *
 * @param fifo_name Path to the FIFO (named pipe) to write the log message.
 * @param message The log message to write.
 */
void log_event(const char *fifo_name, const char *message) {
    pthread_mutex_lock(&log_mutex);
    int fd = open(fifo_name, O_WRONLY | O_NONBLOCK);
    if (fd >= 0) {
        write(fd, message, strlen(message) + ONE);
        close(fd);
    }
    pthread_mutex_unlock(&log_mutex);
}
