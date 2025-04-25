#include "sensor_lib.h"

/**
 * Creates a TCP socket
 * @return socket file descriptor or ERROR on failure
 */
int create_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("[ERROR] - Socket creation failed");
        return ERROR;
    }
    return sock;
}

/**
 * Sets up connection to the server
 * @param client_sock Socket file descriptor
 * @param server_ip Server IP address
 * @param port Server port
 * @return 0 on success, -1 on failure
 */
int setup_server_connection(int client_sock, const char* server_ip, uint16_t port) {
    struct sockaddr_in server_addr;
    
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("[ERROR] - Invalid address or address not supported");
        return ERROR;
    }
    
    // Connect to Gateway
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("[ERROR] - Connection failed");
        return ERROR;
    }
    
    return EXIT_SUCCESS;
}

/**
 * Generates random temperature sensor data
 * @return sensor_data structure with current reading
 */
sensor_data generate_sensor_data() {
    sensor_data data;
    data.temperature = rand() % (MAX_TEMP - MIN_TEMP) + MIN_TEMP;
    data.timestamp = time(NULL);
    return data;
}

/**
 * Sends temperature data to the server
 * @param client_sock Socket file descriptor
 * @param data Temperature data to send
 * @return Number of bytes sent or -1 on failure
 */
int send_temperature_data(int client_sock, sensor_data data) {
    int result = send(client_sock, &data, sizeof(sensor_data), 0);
    if (result < 0) {
        printf("[ERROR] - Send data failed");
    }
    return result;
}

/**
 * Prints the sent temperature data
 * @param data Temperature data that was sent
 */
void print_temp_data(sensor_data data) {
    char *time_str = get_timestamp_string(data.timestamp);
    
    printf("[INFO] - %s - Sensor #%u sent: %u°C\n", time_str, data.id, data.temperature);

    free(time_str);
}

