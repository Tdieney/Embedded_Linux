#include "data_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include "shm_buffer.h"
#include "utils.h"

#define WINDOW_SIZE 5
#define HOT_THRESHOLD 30.0
#define COLD_THRESHOLD 15.0

void *data_mgr_run(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    sensor_data_t data;
    float averages[100] = {0}; // Assume max 100 sensors
    int counts[100] = {0};

    while (1) {
        if (shm_buffer_remove(args->buffer, &data) == 0) {
            if (data.sensor_id < 0 || data.sensor_id >= 100) {
                char msg[64];
                snprintf(msg, sizeof(msg), "Received sensor data with invalid sensor node ID %d", data.sensor_id);
                log_event(args->fifo_name, msg);
                continue;
            }

            // Update running average
            averages[data.sensor_id] = (averages[data.sensor_id] * counts[data.sensor_id] + data.temperature) /
                                      (counts[data.sensor_id] + 1);
            counts[data.sensor_id]++;

            // Log temperature status
            char msg[64];
            if (averages[data.sensor_id] > HOT_THRESHOLD) {
                snprintf(msg, sizeof(msg), "The sensor node with %d reports it's too hot (running avg temperature = %.2f)",
                         data.sensor_id, averages[data.sensor_id]);
                log_event(args->fifo_name, msg);
            } else if (averages[data.sensor_id] < COLD_THRESHOLD) {
                snprintf(msg, sizeof(msg), "The sensor node with %d reports it's too cold (running avg temperature = %.2f)",
                         data.sensor_id, averages[data.sensor_id]);
                log_event(args->fifo_name, msg);
            }
        }
        usleep(100000); // Prevent busy-waiting
    }
    return NULL;
}
