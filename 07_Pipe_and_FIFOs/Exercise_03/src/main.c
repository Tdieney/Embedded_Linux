#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

int main() {
    int fd[2]; // File descriptors for the pipe
    pid_t pid;
    char buf[BUFFER_SIZE];
    
    // Create the pipe
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork a child process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // Child process (reader)
    if (pid == 0) { 
        close(fd[1]); // Close the write end
        read(fd[0], buf, sizeof(buf)); // Read from the pipe
        int msg_len = strlen(buf);
        printf("Child received: %s\nMessage length: %d\n", buf, msg_len);
        close(fd[0]); // Close the read end
    }
    // Parent process (writer) 
    else { 
        close(fd[0]); // Close the read end
        char *msg = "Hello from parent!";
        write(fd[1], msg, strlen(msg) + 1); // Write to the pipe
        close(fd[1]); // Close the write end
        wait(NULL); // Wait for the child to finish
    }

    return 0;
}