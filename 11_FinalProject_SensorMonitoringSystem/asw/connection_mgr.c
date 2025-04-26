#include "connection_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "shm_buffer.h"
#include "utils.h"
#include "common.h"

#define LISTEN_BACKLOG  50U

/**
 * @brief Formats the temperature data
 * @param msg Client IP address
 * @param data Temperature data received from client
 * @param client_ip Client IP address
 */
void format_temperature_data(char *msg, sensor_data data, const char *client_ip) {
    char *time_str = get_timestamp_string(data.timestamp);
    
    sprintf(msg, "%s - Sensor #%u (%s): %u°C", 
           time_str, 
           data.id,
           client_ip, 
           data.temperature);
    
    free(time_str);
}

/**
 * @brief TCP server thread to accept sensor node connections, 
 * receive temperature data, log events, and store data to shared memory.
 *
 * @param arg Pointer to thread_args_t containing port, fifo name, and shared memory buffer.
 * @return NULL
 */
void *connection_mgr_run(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    sensor_data data;
    int opt = ONE;

    // TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("[ERROR] - Failed to create socket");
        log_event(args->fifo_name, "Failed to create socket");
        return NULL;
    }

    // Set socket options (reuse address and port)
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[ERROR] - Setsockopt failed");
        close(server_fd);
        return NULL;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(args->port);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 ||
        listen(server_fd, LISTEN_BACKLOG) < 0) {
        perror("[ERROR] - Bind or listen failed");
        log_event(args->fifo_name, "Bind or listen failed");
        close(server_fd);
        return NULL;
    }
    printf("[INFO] - TCP server is available.\n");

    while (ONE) {
        char msg[64] = {0};
        ssize_t bytes_received;

        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            // perror("[ERROR] - Accept failed.");
            continue;
        }

        // close(server_fd);
        
        // Read sensor data
        while (recv(client_fd, &data, sizeof(sensor_data), 0) > 0) {
            if (data.id <= 0) {
                sprintf(msg, "Received sensor data with invalid sensor node ID %d", data.id);
                log_event(args->fifo_name, msg);
                continue;
            }

            CLEAR_ARRAY(msg, sizeof(msg));
            format_temperature_data(msg, data, (char *)inet_ntoa(client_addr.sin_addr));
            log_event(args->fifo_name, msg);

            if (shm_buffer_insert(args->buffer, &data) < 0) {
                log_event(args->fifo_name, "Shared memory buffer full");
            }
        }

        CLEAR_ARRAY(msg, sizeof(msg));
        sprintf(msg, "The sensor node with %d has closed the connection", data.id);
        log_event(args->fifo_name, msg);
        close(client_fd);
    }   

    close(server_fd);
    return NULL;
}
