#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "shm_buffer.h"
#include "connection_mgr.h"
#include "data_mgr.h"
#include "storage_mgr.h"
#include "log_process.h"

#define FIFO_NAME "logFifo"
#define SHM_NAME "/sensor_shm"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    shm_buffer_t *shm_buffer = shm_buffer_init(SHM_NAME);
    if (!shm_buffer) {
        fprintf(stderr, "Failed to initialize shared memory buffer\n");
        return 1;
    }

    // Create FIFO if it doesn't exist
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        printf("Failed to create FIFO");
        shm_buffer_free(SHM_NAME, shm_buffer);
        return 1;
    }

    // Fork log process
    pid_t pid = fork();
    if (pid < 0) {
        printf("Fork failed");
        shm_buffer_free(SHM_NAME, shm_buffer);
        return 1;
    } else if (pid == 0) {
        // Child: Run log process
        log_process_run(FIFO_NAME);
        exit(0);
    }

    // Parent: Initialize threads
    pthread_t conn_thread, data_thread, storage_thread;
    thread_args_t args = {shm_buffer, port, FIFO_NAME};

    if (pthread_create(&conn_thread, NULL, connection_mgr_run, &args) != 0 ||
        pthread_create(&data_thread, NULL, data_mgr_run, &args) != 0 ||
        pthread_create(&storage_thread, NULL, storage_mgr_run, &args) != 0) {
        fprintf(stderr, "Thread creation failed\n");
        kill(pid, SIGTERM);
        shm_buffer_free(SHM_NAME, shm_buffer);
        return 1;
    }

    // Wait for threads to finish (optional for cleanup)
    pthread_join(conn_thread, NULL);
    pthread_join(data_thread, NULL);
    pthread_join(storage_thread, NULL);

    // Cleanup
    kill(pid, SIGTERM);
    shm_buffer_free(SHM_NAME, shm_buffer);
    unlink(FIFO_NAME);
    return 0;
}
