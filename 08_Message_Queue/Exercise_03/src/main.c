#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#define QUEUE_NAME      "/my_mq"
#define MAX_MESSAGE     10U
#define MAX_MSG_SIZE    256U

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];
    pid_t pid1, pid2;

    // Initialize message queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGE;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Create message queue
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    // Fork first child
    pid1 = fork();
    if (pid1 < 0) {
        perror("fork pid1");
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
        exit(1);
    }

    if (pid1 == 0) { // Child 1
        // Receive message from parent
        ssize_t bytes_read = mq_receive(mq, buffer, MAX_MSG_SIZE, NULL);
        if (bytes_read == -1) {
            perror("child1 mq_receive");
            mq_close(mq);
            exit(1);
        }

        buffer[bytes_read] = '\0';
        printf("Child 1 received: %s\n", buffer);

        // Convert to uppercase
        for (int i = 0; buffer[i]; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        printf("Child 1 converted to: %s\n", buffer);

        // Send uppercase string back to queue
        if (mq_send(mq, buffer, strlen(buffer) + 1, 0) == -1) {
            perror("child1 mq_send");
            mq_close(mq);
            exit(1);
        }

        // Close queue
        mq_close(mq);
        exit(0);
    }

    // Fork second child
    pid2 = fork();
    if (pid2 < 0) {
        perror("fork pid2");
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
        exit(1);
    }

    if (pid2 == 0) { // Child 2
        // Receive message from child 1
        ssize_t bytes_read = mq_receive(mq, buffer, MAX_MSG_SIZE, NULL);
        if (bytes_read == -1) {
            perror("child2 mq_receive");
            mq_close(mq);
            exit(1);
        }

        buffer[bytes_read] = '\0';
        printf("Child 2 received and prints: %s\n", buffer);

        // Close queue
        mq_close(mq);
        exit(0);
    }

    // Parent process
    const char *message = "Hello from parent!";
    
    // Send message to child 1
    if (mq_send(mq, message, strlen(message) + 1, 0) == -1) {
        perror("parent mq_send");
    }
    printf("Parent sent: %s\n", message);

    // Wait for both children to finish
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    // Close and unlink queue
    mq_close(mq);
    mq_unlink(QUEUE_NAME);

    return 0;
}