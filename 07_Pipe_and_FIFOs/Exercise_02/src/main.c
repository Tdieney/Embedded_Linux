#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

int main() {
    int pipe1[2]; // File descriptor for the pipe from parent to child1
    int pipe2[2]; // File descriptor for the pipe from child1 to child2
    pid_t pid1, pid2;
    
    // Create the pipe1
    if (pipe(pipe1) == -1) {
        perror("Pipe 1 creation failed");
        exit(EXIT_FAILURE);
    }

    // Fork a child1 process
    pid1 = fork();
    if (pid1 == -1) {
        perror("Fork 1 failed");
        exit(EXIT_FAILURE);
    }

    // Child1 - reader
    if (pid1 == 0) { 
        close(pipe1[1]);

        char child1_msg[BUFFER_SIZE];
        read(pipe1[0], child1_msg, sizeof(child1_msg));
        printf("Child1 received: %s\n", child1_msg);

        strcat(child1_msg, " - Modified by Child 1");
        close(pipe1[0]);

        // Create the pipe2
        if (pipe(pipe2) == -1) {
            perror("Pipe 2 creation failed");
            exit(EXIT_FAILURE);
        }

        // Fork a child2 process
        pid2 = fork();
        if (pid2 == -1) {
            perror("Fork 2 failed");
            exit(EXIT_FAILURE);
        }

        // Child2 - reader
        if (pid2 == 0) {
            close(pipe2[1]);
            char child2_msg[BUFFER_SIZE];
            read(pipe2[0], child2_msg, sizeof(child2_msg));
            printf("Child2 received: %s\n", child2_msg);
            close(pipe2[0]);
        }
        // Child1 - writer
        else {
            close(pipe2[0]);
            write(pipe2[1], child1_msg, strlen(child1_msg) + 1);
            printf("Child1 sent: %s\n", child1_msg);
            close(pipe2[1]);
            wait(NULL);
        }
    }
    // Parent - writer
    else { 
        close(pipe1[0]);
        char *msg = "Hello from parent!";
        write(pipe1[1], msg, strlen(msg) + 1);
        printf("Parent sent: %s\n", msg);
        close(pipe1[1]);
        wait(NULL);
    }

    return 0;
}