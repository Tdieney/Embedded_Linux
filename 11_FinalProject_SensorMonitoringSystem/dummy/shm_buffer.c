#include "shm_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 100U

struct shm_buffer {
    pthread_mutex_t mutex;
    size_t head;
    size_t tail;
    size_t count;
    sensor_data_t data[BUFFER_SIZE];
};

shm_buffer_t *shm_buffer_init(const char *name) {
    // Create or open shared memory object
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("shm_open failed");
        return NULL;
    }

    // Set size of shared memory
    if (ftruncate(fd, sizeof(shm_buffer_t)) < 0) {
        perror("ftruncate failed");
        close(fd);
        return NULL;
    }

    // Map shared memory
    shm_buffer_t *buffer = mmap(NULL, sizeof(shm_buffer_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (buffer == MAP_FAILED) {
        perror("mmap failed");
        return NULL;
    }

    // Initialize mutex (only once)
    static int initialized = 0;
    if (!initialized) {
        pthread_mutexattr_t mutex_attr;
        pthread_mutexattr_init(&mutex_attr);
        pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&buffer->mutex, &mutex_attr);
        pthread_mutexattr_destroy(&mutex_attr);
        buffer->head = 0;
        buffer->tail = 0;
        buffer->count = 0;
        initialized = 1;
    }

    return buffer;
}

void shm_buffer_free(const char *name, shm_buffer_t *buffer) {
    if (!buffer) return;
    pthread_mutex_destroy(&buffer->mutex);
    munmap(buffer, sizeof(shm_buffer_t));
    shm_unlink(name);
}

int shm_buffer_insert(shm_buffer_t *buffer, sensor_data_t *data) {
    pthread_mutex_lock(&buffer->mutex);
    if (buffer->count >= BUFFER_SIZE) {
        pthread_mutex_unlock(&buffer->mutex);
        return -1; // Buffer full
    }

    buffer->data[buffer->tail] = *data;
    buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
    buffer->count++;
    pthread_mutex_unlock(&buffer->mutex);
    return 0;
}

int shm_buffer_remove(shm_buffer_t *buffer, sensor_data_t *data) {
    pthread_mutex_lock(&buffer->mutex);
    if (buffer->count == 0) {
        pthread_mutex_unlock(&buffer->mutex);
        return -1; // Buffer empty
    }

    *data = buffer->data[buffer->head];
    buffer->head = (buffer->head + 1) % BUFFER_SIZE;
    buffer->count--;
    pthread_mutex_unlock(&buffer->mutex);
    return 0;
}
