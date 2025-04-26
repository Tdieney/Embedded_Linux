#ifndef SHM_BUFFER_H
#define SHM_BUFFER_H

#include <pthread.h>
#include <time.h>
#include "sensor_lib.h"

#define MAX_SENSOR_BUFFER 100U

typedef struct {
    pthread_mutex_t mutex;
    size_t head;
    size_t tail;
    size_t count;
    sensor_data data[MAX_SENSOR_BUFFER];
} shm_buffer_t;

typedef struct {
    shm_buffer_t *buffer;
    int port;
    const char *fifo_name;
} thread_args_t;

/**
 * @brief Initialize a POSIX shared memory buffer for inter-process communication.
 *
 * @param name Name of the shared memory object.
 * @return Pointer to the mapped shm_buffer_t structure, or NULL on failure.
 */
shm_buffer_t *shm_buffer_init(const char *name);

/**
 * @brief Unmap and remove a POSIX shared memory buffer.
 *
 * @param name Name of the shared memory object to unlink.
 * @param buffer Pointer to the mapped shm_buffer_t to unmap and destroy.
 */
void shm_buffer_free(const char *name, shm_buffer_t *buffer);

/**
 * @brief Insert a sensor data item into the shared memory buffer (thread-safe).
 *
 * @param buffer Pointer to the shared memory buffer.
 * @param data Pointer to the sensor data to insert.
 * @return EXIT_SUCCESS on success, or ERROR if the buffer is full.
 */
int shm_buffer_insert(shm_buffer_t *buffer, sensor_data *data);

/**
 * @brief Remove a sensor data item from the shared memory buffer (thread-safe).
 *
 * @param buffer Pointer to the shared memory buffer.
 * @param data Pointer to store the removed sensor data.
 * @return EXIT_SUCCESS on success, or ERROR if the buffer is empty.
 */
int shm_buffer_remove(shm_buffer_t *buffer, sensor_data *data);

#endif // SHM_BUFFER_H
