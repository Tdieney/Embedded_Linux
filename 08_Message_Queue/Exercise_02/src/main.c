#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/wait.h>

#define QUEUE_NAME_MSG      "/my_mq_msg"
#define QUEUE_NAME_COUNT    "/my_mq_count"
#define MAX_MESSAGE         10U
#define MAX_MSG_SIZE        256U

int main() {
    mqd_t mq_msg, mq_count;
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];
    pid_t pid;

    // Initialize message queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGE;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Create message queue for sending string
    mq_msg = mq_open(QUEUE_NAME_MSG, O_CREAT | O_RDWR, 0644, &attr);
    if (mq_msg == (mqd_t)-1) {
        printf("mq_open msg");
        exit(1);
    }

    // Create message queue for sending count
    mq_count = mq_open(QUEUE_NAME_COUNT, O_CREAT | O_RDWR, 0644, &attr);
    if (mq_count == (mqd_t)-1) {
        printf("mq_open count");
        mq_close(mq_msg);
        mq_unlink(QUEUE_NAME_MSG);
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        printf("fork");
        mq_close(mq_msg);
        mq_close(mq_count);
        mq_unlink(QUEUE_NAME_MSG);
        mq_unlink(QUEUE_NAME_COUNT);
        exit(1);
    }

    if (pid > 0) { // Parent process
        const char *message = "Hello from parent!";
        
        // Send message to child
        if (mq_send(mq_msg, message, strlen(message) + 1, 0) == -1) {
            printf("mq_send msg");
        }

        // Receive count from child
        ssize_t bytes_read = mq_receive(mq_count, buffer, MAX_MSG_SIZE, NULL);
        if (bytes_read == -1) {
            printf("mq_receive count");
        } else {
            buffer[bytes_read] = '\0';
            printf("Parent received: Number of characters = %s\n", buffer);
        }

        // Wait for child to finish
        wait(NULL);

        // Close and unlink queues
        mq_close(mq_msg);
        mq_close(mq_count);
        mq_unlink(QUEUE_NAME_MSG);
        mq_unlink(QUEUE_NAME_COUNT);
    } else { // Child process
        // Receive message from parent
        ssize_t bytes_read = mq_receive(mq_msg, buffer, MAX_MSG_SIZE, NULL);
        if (bytes_read == -1) {
            printf("mq_receive msg");
            exit(1);
        }

        buffer[bytes_read] = '\0';
        printf("Child received: %s\n", buffer);

        // Count characters (excluding null terminator)
        int char_count = strlen(buffer);
        char count_str[16];
        snprintf(count_str, sizeof(count_str), "%d", char_count);

        // Send count to parent
        if (mq_send(mq_count, count_str, strlen(count_str) + 1, 0) == -1) {
            printf("mq_send count");
        }

        // Close queues
        mq_close(mq_msg);
        mq_close(mq_count);
        exit(0);
    }

    return 0;
}