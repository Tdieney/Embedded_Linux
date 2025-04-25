#ifndef SBUFFER_H
#define SBUFFER_H

#include <pthread.h>
#include <time.h>

typedef struct {
    int sensor_id;
    float temperature;
    time_t timestamp;
    struct sensor_data *next;
} sensor_data_t;

typedef struct sbuffer sbuffer_t;

sbuffer_t *sbuffer_init(void);
void sbuffer_free(sbuffer_t *sbuffer);
int sbuffer_insert(sbuffer_t *sbuffer, sensor_data_t *data);
int sbuffer_remove(sbuffer_t *sbuffer, sensor_data_t *data);

typedef struct {
    sbuffer_t *sbuffer;
    int port;
    const char *fifo_name;
} thread_args_t;

#endif // SBUFFER_H
