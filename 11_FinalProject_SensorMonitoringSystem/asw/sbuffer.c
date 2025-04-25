#include "sbuffer.h"
#include <stdlib.h>
#include <pthread.h>

struct sbuffer {
    sensor_data_t *head;
    sensor_data_t *tail;
    pthread_mutex_t mutex;
};

sbuffer_t *sbuffer_init(void) {
    sbuffer_t *sbuffer = malloc(sizeof(sbuffer_t));
    if (!sbuffer) return NULL;
    sbuffer->head = sbuffer->tail = NULL;
    pthread_mutex_init(&sbuffer->mutex, NULL);
    return sbuffer;
}

void sbuffer_free(sbuffer_t *sbuffer) {
    if (!sbuffer) return;
    pthread_mutex_lock(&sbuffer->mutex);
    sensor_data_t *current = sbuffer->head;
    while (current) {
        sensor_data_t *next = current->next;
        free(current);
        current = next;
    }
    pthread_mutex_unlock(&sbuffer->mutex);
    pthread_mutex_destroy(&sbuffer->mutex);
    free(sbuffer);
}

int sbuffer_insert(sbuffer_t *sbuffer, sensor_data_t *data) {
    sensor_data_t *new_data = malloc(sizeof(sensor_data_t));
    if (!new_data) return -1;
    *new_data = *data;
    new_data->next = NULL;

    pthread_mutex_lock(&sbuffer->mutex);
    if (!sbuffer->tail) {
        sbuffer->head = sbuffer->tail = new_data;
    } else {
        sbuffer->tail->next = new_data;
        sbuffer->tail = new_data;
    }
    pthread_mutex_unlock(&sbuffer->mutex);
    return 0;
}

int sbuffer_remove(sbuffer_t *sbuffer, sensor_data_t *data) {
    pthread_mutex_lock(&sbuffer->mutex);
    if (!sbuffer->head) {
        pthread_mutex_unlock(&sbuffer->mutex);
        return -1;
    }
    sensor_data_t *current = sbuffer->head;
    sbuffer->head = current->next;
    if (!sbuffer->head) sbuffer->tail = NULL;
    *data = *current;
    free(current);
    pthread_mutex_unlock(&sbuffer->mutex);
    return 0;
}
