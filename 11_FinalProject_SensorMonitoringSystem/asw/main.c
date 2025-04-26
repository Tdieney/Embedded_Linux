#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>
#include "shm_buffer.h"
#include "connection_mgr.h"
#include "data_mgr.h"
#include "storage_mgr.h"
#include "log_process.h"
#include "common.h"

#define FIFO_NAME "/tmp/fifo_log"
#define SHM_NAME "/shared_data"

uint8_t main(int argc, char *argv[]) {
    uint8_t ret_val = EXIT_FAILURE;
    int status;

    if (argc != TWO) {
        printf("[ERROR] - Usage: %s <port>", argv[0]);
        return ret_val;
    }

    int port = atoi(argv[1]);
    shm_buffer_t *shm_buffer = shm_buffer_init(SHM_NAME);
    if (!shm_buffer) {
        perror("[ERROR] - Failed to initialize shared memory");
        return ret_val;
    }
    printf("[INFO] - Shared memory is initialized successfully.\n");

    unlink(FIFO_NAME); // Delete old FIFO if it existed
    if (mkfifo(FIFO_NAME, 0666) == ERROR) {
        perror("[ERROR] - Failed to create FIFO");
        shm_buffer_free(SHM_NAME, shm_buffer);
        return ret_val;
    }

    // Fork log process
    pid_t pid = fork();
    if (pid < 0) {
        perror("[ERROR] - Fork failed");
        shm_buffer_free(SHM_NAME, shm_buffer);
        return ret_val;
    } else if (pid == 0) {
        // Child: Run log process
        ret_val = log_process_run(FIFO_NAME);
        exit(ret_val);
    }

    // Parent: Initialize threads
    pthread_t connection_mgr, data_mgr, storage_mgr;
    thread_args_t args = {shm_buffer, port, FIFO_NAME};

    if (pthread_create(&connection_mgr, NULL, connection_mgr_run, &args) != 0 ||
        pthread_create(&data_mgr, NULL, data_mgr_run, &args) != 0 ||
        pthread_create(&storage_mgr, NULL, storage_mgr_run, &args) != 0) {
        perror("[ERROR] - Thread creation failed");
        kill(pid, SIGTERM);
        shm_buffer_free(SHM_NAME, shm_buffer);
        return EXIT_FAILURE;
    }

    // Wait for threads to finish
    pthread_join(connection_mgr, NULL);
    pthread_join(data_mgr, NULL);
    // pthread_join(storage_mgr, NULL);

    // Cleanup
    waitpid(pid, &status, 0);
    shm_buffer_free(SHM_NAME, shm_buffer);
    unlink(FIFO_NAME);

    return EXIT_SUCCESS;
}
