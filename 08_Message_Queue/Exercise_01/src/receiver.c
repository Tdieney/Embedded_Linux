#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define MAX_SIZE            128U
#define MAX_MESSAGES        10U

volatile sig_atomic_t done = 0;

void handle_signal(int sig) {
    done = 1;
}

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE + 1];
    unsigned int prio;
    ssize_t bytes_read;
    const char *q_name = "/my_queue";

    // Signal handling
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    // Open queue (non-blocking)
    mq = mq_open(QUEUE_NAME, O_RDONLY | O_NONBLOCK);
    if (mq == (mqd_t)-1) {
        printf("mq_open failed");
        exit(EXIT_FAILURE);
    }

    // Get attributes
    if (mq_getattr(mq, &attr) == -1) {
        printf("mq_getattr failed");
        mq_close(mq);
        exit(EXIT_FAILURE);
    }
    printf("Queue \"%s\":\n  Max messages: %ld\n  Max size: %ld\n  Current messages: %ld\n",
           QUEUE_NAME, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

    printf("Waiting for messages (Ctrl+C to quit)...\n");

    while (!done) {
        bytes_read = mq_receive(mq, buffer, MAX_SIZE, &prio);
        if (bytes_read == -1) {
            if (errno == EAGAIN) {
                usleep(100000); // Wait 100ms before retrying
                continue;
            } else if (errno == EINTR) {
                continue;
            }
            printf("mq_receive failed");
            break;
        }

        buffer[bytes_read] = '\0';
        printf("Received (priority %u): %s\n", prio, buffer);

        if (strncmp(buffer, "quit", 4) == 0) {
            break;
        }
    }

    if (mq_close(mq) == -1) {
        printf("mq_close failed");
    }

    if (mq_unlink(QUEUE_NAME) == -1) {
        printf("mq_unlink failed");
    }

    printf("Queue removed\n");
    return 0;
}