#ifndef SHM_BUFFER_H
#define SHM_BUFFER_H

#include <pthread.h>
#include <time.h>
#include "sensor_lib.h"

typedef struct shm_buffer shm_buffer_t;

shm_buffer_t *shm_buffer_init(const char *name);
void shm_buffer_free(const char *name, shm_buffer_t *buffer);
int shm_buffer_insert(shm_buffer_t *buffer, SensorData *data);
int shm_buffer_remove(shm_buffer_t *buffer, SensorData *data);

typedef struct {
    shm_buffer_t *buffer;
    int port;
    const char *fifo_name;
} thread_args_t;

#endif // SHM_BUFFER_H
