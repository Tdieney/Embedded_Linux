#include "data_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include "shm_buffer.h"
#include "utils.h"

#define WINDOW_SIZE 5U
#define HOT_THRESHOLD 30U
#define COLD_THRESHOLD 15U

void *data_mgr_run(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    sensor_data data;
    float averages[100] = {0}; // Assume max 100 sensors
    int counts[100] = {0};

    while (1) {
        if (shm_buffer_remove(args->buffer, &data) == 0) {
            if (data.id < 0 || data.id >= 100) {
                char msg[64];
                snprintf(msg, sizeof(msg), "Received sensor data with invalid sensor node ID %d", data.id);
                log_event(args->fifo_name, msg);
                continue;
            }

            // Update running average
            // averages[data.id] = (averages[data.id] * counts[data.id] + data.temperature) /
            //                           (counts[data.id] + 1);
            // counts[data.id]++;

            // Log temperature status
            char msg[100];
            // if (averages[data.id] > HOT_THRESHOLD) {
            if (data.temperature > HOT_THRESHOLD) {
                snprintf(msg, sizeof(msg), "The sensor node with %u reports it's too hot (running avg temperature = %u)",
                         data.id, data.temperature);
                log_event(args->fifo_name, msg);
            // } else if (averages[data.id] < COLD_THRESHOLD) {
            } else if (data.temperature < COLD_THRESHOLD) {
                snprintf(msg, sizeof(msg), "The sensor node with %u reports it's too cold (running avg temperature = %u)",
                         data.id, data.temperature);
                log_event(args->fifo_name, msg);
            }
        }
        usleep(100000); // Prevent busy-waiting
    }
    return NULL;
}
