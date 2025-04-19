#ifndef SENSOR_LIB_H
#define SENSOR_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <types.h>

#define SERVER_IP   "127.0.0.1"
#define MAX_TEMP    40U
#define MIN_TEMP    10U

typedef struct {
    uint16 id;
    uint16 temperature;
    time_t timestamp;
} SensorData;

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
int setup_server_connection(int client_sock, const char* server_ip, uint16 port);

/**
 * Generates random temperature sensor data
 * @return SensorData structure with current reading
 */
SensorData generate_sensor_data();

/**
 * Sends temperature data to the server
 * @param client_sock Socket file descriptor
 * @param data Temperature data to send
 * @return Number of bytes sent or -1 on failure
 */
int send_temperature_data(int client_sock, SensorData data);

/**
 * Prints the sent temperature data
 * @param data Temperature data that was sent
 */
void print_temp_data(SensorData data);

#endif // SENSOR_LIB_H
