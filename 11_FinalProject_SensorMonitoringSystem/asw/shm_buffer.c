#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"
#include "shm_buffer.h"

/**
 * @brief Initialize a POSIX shared memory buffer for inter-process communication.
 *
 * @param name Name of the shared memory object.
 * @return Pointer to the mapped shm_buffer_t structure, or NULL on failure.
 */
shm_buffer_t *shm_buffer_init(const char *name) {
    shm_unlink(name); // Try to unlink old shared memory
    // Create or open shared memory object
    int fd = shm_open(name, O_CREAT | O_RDWR, PERM_RW_ALL);
    if (fd < 0) {
        perror("[ERROR] - shm_open failed");
        return NULL;
    }

    // Set size of shared memory
    if (ftruncate(fd, sizeof(shm_buffer_t)) < 0) {
        perror("[ERROR] - ftruncate failed");
        close(fd);
        return NULL;
    }

    // Map shared memory
    shm_buffer_t *buffer = mmap(NULL, sizeof(shm_buffer_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (buffer == MAP_FAILED) {
        perror("[ERROR] - mmap failed");
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

/**
 * @brief Unmap and remove a POSIX shared memory buffer.
 *
 * @param name Name of the shared memory object to unlink.
 * @param buffer Pointer to the mapped shm_buffer_t to unmap and destroy.
 */
void shm_buffer_free(const char *name, shm_buffer_t *buffer) {
    if (!buffer) return;
    pthread_mutex_destroy(&buffer->mutex);
    munmap(buffer, sizeof(shm_buffer_t));
    shm_unlink(name);
}

/**
 * @brief Insert a sensor data item into the shared memory buffer (thread-safe).
 *
 * @param buffer Pointer to the shared memory buffer.
 * @param data Pointer to the sensor data to insert.
 * @return EXIT_SUCCESS on success, or ERROR if the buffer is full.
 */
int shm_buffer_insert(shm_buffer_t *buffer, sensor_data *data) {
    pthread_mutex_lock(&buffer->mutex);
    if (buffer->count >= MAX_SENSOR_BUFFER) {
        pthread_mutex_unlock(&buffer->mutex);
        perror("[ERROR] - Shared data is full.");
        return ERROR;
    }

    buffer->data[buffer->tail] = *data;
    buffer->tail = (buffer->tail + ONE) % MAX_SENSOR_BUFFER;
    buffer->count++;
    pthread_mutex_unlock(&buffer->mutex);
    return EXIT_SUCCESS;
}

/**
 * @brief Remove a sensor data item from the shared memory buffer (thread-safe).
 *
 * @param buffer Pointer to the shared memory buffer.
 * @param data Pointer to store the removed sensor data.
 * @return EXIT_SUCCESS on success, or ERROR if the buffer is empty.
 */
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
    return EXIT_SUCCESS;
}
