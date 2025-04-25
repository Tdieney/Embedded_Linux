#ifndef SENSOR_LIB_H
#define SENSOR_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include "common.h"

#define SERVER_IP   "127.0.0.1"
#define MAX_TEMP    40U
#define MIN_TEMP    10U

#define TIMER_100MS 100000UL

typedef struct {
    uint16_t id;
    uint16_t temperature;
    time_t timestamp;
} sensor_data;

/**
 * Creates a TCP socket
 * @return socket file descriptor or -1 on failure
 */
int create_socket();

/**
 * Sets up connection to the server
 * @param client_sock Socket file descriptor
 * @param server_ip Server IP address
 * @param port Server port
 * @return 0 on success, -1 on failure
 */
int setup_server_connection(int client_sock, const char* server_ip, uint16_t port);

/**
 * Generates random temperature sensor data
 * @return sensor_data structure with current reading
 */
sensor_data generate_sensor_data();

/**
 * Sends temperature data to the server
 * @param client_sock Socket file descriptor
 * @param data Temperature data to send
 * @return Number of bytes sent or -1 on failure
 */
int send_temperature_data(int client_sock, sensor_data data);

/**
 * Prints the sent temperature data
 * @param data Temperature data that was sent
 */
void print_temp_data(sensor_data data);

#endif // SENSOR_LIB_H
