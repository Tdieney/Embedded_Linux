#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <sensor_lib.h>
#include <common.h>

/**
 * Sets up the server socket to listen for connections
 * @param port The port to listen on
 * @return Socket file descriptor or -1 on failure
 */
int setup_server_socket(uint16_t port);

/**
 * Handles communication with a connected client
 * @param client_sock Client socket file descriptor
 * @param client_addr Client address information
 */
void process_client_connection(int client_sock, struct sockaddr_in *client_addr);

/**
 * Prints the temperature data in a formatted way
 * @param data Temperature data received from client
 * @param client_ip Client IP address
 */
void print_temperature_data(SensorData data, const char *client_ip);

#endif // MAIN_H
