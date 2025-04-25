#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>
#include <errno.h>

#define MAX_SIZE            128U
#define MAX_MESSAGES        10U
#define QUEUE_PERMISSIONS   0660


int main() {
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE + 1];
    int must_stop = 0;
    const char *q_name = "/my_queue";

    // Set attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    // Create queue
    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, QUEUE_PERMISSIONS, &attr);
    if (mq == (mqd_t)-1) {
        printf("mq_open failed");
        exit(EXIT_FAILURE);
    }

    do {
        printf("Enter message (or empty to quit): ");
        if (fgets(buffer, MAX_SIZE, stdin) == NULL) {
            printf("fgets failed");
            break;
        }

        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }

        if (len == 0) {
            must_stop = 1;
        } else {
            if (mq_send(mq, buffer, len + 1, 1) == -1) {
                printf("mq_send failed");
                break;
            }
            printf("Message sent: %s\n", buffer);
        }
    } while (!must_stop);

    if (mq_close(mq) == -1) {
        printf("mq_close failed");
        exit(EXIT_FAILURE);
    }

    printf("Queue closed\n");
    return 0;
}