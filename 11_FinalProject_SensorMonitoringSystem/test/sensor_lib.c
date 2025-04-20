#include "sensor_lib.h"

/**
 * Creates a TCP socket
 * @return socket file descriptor or -1 on failure
 */
int create_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
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
        perror("Invalid address or address not supported");
        return -1;
    }
    
    // Connect to Gateway
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }
    
    return 0;
}

/**
 * Generates random temperature sensor data
 * @return SensorData structure with current reading
 */
SensorData generate_sensor_data() {
    SensorData data;
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
int send_temperature_data(int client_sock, SensorData data) {
    int result = send(client_sock, &data, sizeof(SensorData), 0);
    if (result < 0) {
        perror("Send failed");
    }
    return result;
}

/**
 * Prints the sent temperature data
 * @param data Temperature data that was sent
 */
void print_temp_data(SensorData data) {
    char *time_str = get_timestamp_string(data.timestamp);
    
    printf("%s - Sensor #%u - Data sent: %u°C\n", time_str, data.id, data.temperature);

    free(time_str);

    // Flush stdout to ensure immediate display
    fflush(stdout);
}

