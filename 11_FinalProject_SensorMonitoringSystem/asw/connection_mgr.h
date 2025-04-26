#ifndef CONNECTION_MGR_H
#define CONNECTION_MGR_H

#include "sensor_lib.h"

/**
 * @brief Formats the temperature data
 * @param msg Client IP address
 * @param data Temperature data received from client
 * @param client_ip Client IP address
 */
void format_temperature_data(char *msg, sensor_data data, const char *client_ip);

/**
 * @brief TCP server thread to accept sensor node connections, 
 * receive temperature data, log events, and store data to shared memory.
 *
 * @param arg Pointer to thread_args_t containing port, fifo name, and shared memory buffer.
 * @return NULL
 */
void *connection_mgr_run(void *arg);

#endif // CONNECTION_MGR_H
