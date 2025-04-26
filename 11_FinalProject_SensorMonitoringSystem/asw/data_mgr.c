#include "data_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include "shm_buffer.h"
#include "utils.h"
#include "common.h"

#define WINDOW_SIZE     5U
#define HOT_THRESHOLD   30U
#define COLD_THRESHOLD  15U
#define MSG_SIZE        100U

/**
 * @brief Thread function to process sensor data from shared memory,
 * log temperature status, and maintain running averages.
 *
 * @param arg Pointer to thread_args_t containing FIFO name and shared memory buffer.
 * @return NULL
 */
void *data_mgr_run(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    sensor_data data;
    float averages[MAX_SENSOR_BUFFER] = {0}; // Assume max 100 sensors
    int counts[MAX_SENSOR_BUFFER] = {0};

    while (ONE) {
        if (shm_buffer_remove(args->buffer, &data) == 0) {
            char msg[MSG_SIZE];

            if (data.id < 0 || data.id >= MAX_SENSOR_BUFFER) {
                CLEAR_ARRAY(msg, sizeof(msg));
                sprintf(msg, "Received sensor data with invalid sensor node ID %d", data.id);
                log_event(args->fifo_name, msg);
                continue;
            }

            // Log temperature status
            if (data.temperature > HOT_THRESHOLD) {
                CLEAR_ARRAY(msg, sizeof(msg));
                sprintf(msg, "Sensor #%u reports it's too hot (recent temperature = %u°C)",
                         data.id, data.temperature);
                log_event(args->fifo_name, msg);
            } else if (data.temperature < COLD_THRESHOLD) {
                CLEAR_ARRAY(msg, sizeof(msg));
                sprintf(msg, "Sensor #%u reports it's too cold (recent temperature = %u°C)",
                         data.id, data.temperature);
                log_event(args->fifo_name, msg);
            }
        }
        usleep(DELAY_100MS); // Prevent busy-waiting
    }
    return NULL;
}
