#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"
#include "shm_buffer.h"

shm_buffer_t *shm_buffer_init(const char *name) {
    // Create or open shared memory object
    int fd = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd < 0) {
        printf("[ERROR] - shm_open failed");
        return NULL;
    }

    // Set size of shared memory
    if (ftruncate(fd, sizeof(shm_buffer_t)) < 0) {
        printf("[ERROR] - ftruncate failed");
        close(fd);
        return NULL;
    }

    // Map shared memory
    shm_buffer_t *buffer = mmap(NULL, sizeof(shm_buffer_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (buffer == MAP_FAILED) {
        printf("[ERROR] - mmap failed");
        return NULL;
    }

    // Initialize mutex (only once)
    static int initialized = false;
    if (initialized != true) {
        pthread_mutexattr_t mutex_attr;
        pthread_mutexattr_init(&mutex_attr);
        pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&buffer->mutex, &mutex_attr);
        pthread_mutexattr_destroy(&mutex_attr);
        buffer->head = 0;
        buffer->tail = 0;
        buffer->count = 0;
        initialized = true;
    }

    return buffer;
}

void shm_buffer_free(const char *name, shm_buffer_t *buffer) {
    if (!buffer) return;
    pthread_mutex_destroy(&buffer->mutex);
    munmap(buffer, sizeof(shm_buffer_t));
    shm_unlink(name);
}

int shm_buffer_insert(shm_buffer_t *buffer, sensor_data *data) {
    pthread_mutex_lock(&buffer->mutex);
    if (buffer->count >= MAX_SENSOR_BUFFER) {
        pthread_mutex_unlock(&buffer->mutex);
        printf("[ERROR] - Shared data is full.");
        return ERROR;
    }

    buffer->data[buffer->tail] = *data;
    buffer->tail = (buffer->tail + ONE) % MAX_SENSOR_BUFFER;
    buffer->count++;
    pthread_mutex_unlock(&buffer->mutex);
    return 0;
}

int shm_buffer_remove(shm_buffer_t *buffer, sensor_data *data) {
    pthread_mutex_lock(&buffer->mutex);
    if (buffer->count == 0) {
        pthread_mutex_unlock(&buffer->mutex);
        return ERROR; // Buffer empty
    }

    *data = buffer->data[buffer->head];
    buffer->head = (buffer->head + ONE) % MAX_SENSOR_BUFFER;
    buffer->count--;
    pthread_mutex_unlock(&buffer->mutex);
    return 0;
}
