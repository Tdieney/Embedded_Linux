#include "connection_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "shm_buffer.h"
#include "utils.h"

void *connection_mgr_run(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    sensor_data data;

    // Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        log_event(args->fifo_name, "Failed to create socket");
        return NULL;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(args->port);

    // Bind and listen
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 ||
        listen(server_fd, 10) < 0) {
        log_event(args->fifo_name, "Bind or listen failed");
        close(server_fd);
        return NULL;
    }

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) continue;
        char msg[64];

        // Read sensor data
        while (read(client_fd, &data, sizeof(sensor_data)) > 0) {
            if (data.id <= 0) {
                // char msg[64];
                snprintf(msg, sizeof(msg), "Received sensor data with invalid sensor node ID %d", data.id);
                log_event(args->fifo_name, msg);
                continue;
            }

            
            snprintf(msg, sizeof(msg), "A sensor node with %d has opened a new connection", data.id);
            log_event(args->fifo_name, msg);
            // char msg[64];
            if (shm_buffer_insert(args->buffer, &data) < 0) {
                log_event(args->fifo_name, "Shared memory buffer full");
            }
        }

        snprintf(msg, sizeof(msg), "The sensor node with %d has closed the connection", data.id);
        log_event(args->fifo_name, msg);
        close(client_fd);
    }

    close(server_fd);
    return NULL;
}
