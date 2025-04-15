#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_MESSAGE     10U
#define MAX_MSG_SIZE    256U

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];
    pid_t pid;
    const char *q_name = "/my_mq";

    // Initialize message queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGE;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Create message queue
    mq = mq_open(q_name, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        mq_close(mq);
        mq_unlink(q_name);
        exit(1);
    }

    if (pid > 0) { // Parent process
        const char *message = "Hello from parent!";
        
        // Send message
        if (mq_send(mq, message, strlen(message) + 1, 0) == -1) {
            perror("mq_send");
        }

        // Wait for child to finish
        wait(NULL);

        // Close and unlink queue
        mq_close(mq);
        mq_unlink(q_name);
    } else { // Child process
        // Receive message
        ssize_t bytes_read = mq_receive(mq, buffer, MAX_MSG_SIZE, NULL);
        if (bytes_read == -1) {
            perror("mq_receive");
            exit(1);
        }

        buffer[bytes_read] = '\0';
        printf("Child received: %s\n", buffer);

        // Close queue
        mq_close(mq);
        exit(0);
    }

    return 0;
}