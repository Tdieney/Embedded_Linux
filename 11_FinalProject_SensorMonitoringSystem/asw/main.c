#include <main.h>

#define MAX_CLIENTS 10

// Global flag for graceful shutdown
volatile sig_atomic_t running = 1;

// Signal handler for graceful shutdown
void handle_signal(int sig) {
    printf("\nShutdown signal received. Closing server...\n");
    running = 0;
}

int main(int argc, char *argv[]) {
    // Validate command line arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Setup signal handler for graceful shutdown
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    uint16_t port = (uint16_t)atoi(argv[1]);
    int server_sock, client_sock;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int sensor_counter = 1;

    // Create and setup server socket
    server_sock = setup_server_socket(port);
    if (server_sock < 0) {
        return EXIT_FAILURE;
    }

    printf("Temperature Gateway Server started on port %d\n", port);
    printf("Waiting for sensor connections...\n\n");

    // Main server loop
    while (running) {
        // Accept new connection (with timeout to check running flag)
        fd_set readfds;
        struct timeval tv;
        FD_ZERO(&readfds);
        FD_SET(server_sock, &readfds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int activity = select(server_sock + 1, &readfds, NULL, NULL, &tv);
        if (activity < 0) {
            if (!running) break;
            perror("Select error");
            continue;
        }

        if (activity == 0) continue; // Timeout, check running flag again

        if (FD_ISSET(server_sock, &readfds)) {
            client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
            if (client_sock < 0) {
                perror("Accept failed");
                continue;
            }

            // Handle client in a new process (simple multi-client handling)
            pid_t pid = fork();
            if (pid < 0) {
                perror("Fork failed");
                close(client_sock);
            } else if (pid == 0) {
                // Child process
                close(server_sock);
                process_client_connection(client_sock, &client_addr);
                exit(EXIT_SUCCESS);
            } else {
                // Parent process
                close(client_sock);
                sensor_counter++;
            }
        }
    }

    // Clean up
    close(server_sock);
    printf("Server shutdown complete.\n");
    return 0;
}

/**
 * Sets up the server socket to listen for connections
 * @param port The port to listen on
 * @return Socket file descriptor or -1 on failure
 */
int setup_server_socket(uint16_t port) {
    int server_sock;
    struct sockaddr_in server_addr;
    int opt = 1;

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Set socket options (reuse address and port)
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        close(server_sock);
        return -1;
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind socket to address and port
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        return -1;
    }

    // Listen for connections
    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_sock);
        return -1;
    }

    return server_sock;
}

/**
 * Handles communication with a connected client
 * @param client_sock Client socket file descriptor
 * @param client_addr Client address information
 */
void process_client_connection(int client_sock, struct sockaddr_in *client_addr) {
    SensorData temp_data;
    char *client_ip = inet_ntoa(client_addr->sin_addr);
    ssize_t bytes_received;

    if ((bytes_received = recv(client_sock, &temp_data, sizeof(SensorData), 0)) > 0)
    {
        printf("New sensor connected: ID #%d from %s:%d\n", 
            temp_data.id,
            inet_ntoa(client_addr->sin_addr), 
            ntohs(client_addr->sin_port));

        print_temperature_data(temp_data, client_ip);
    }

    // Receive and process data from the client
    while ((bytes_received = recv(client_sock, &temp_data, sizeof(SensorData), 0)) > 0) {
        print_temperature_data(temp_data, client_ip);
    }

    if (bytes_received == 0) {
        printf("Sensor #%d disconnected.\n", temp_data.id);
    } else {
        perror("Receive failed");
    }

    close(client_sock);
}

/**
 * Prints the temperature data in a formatted way
 * @param data Temperature data received from client
 * @param client_ip Client IP address
 */
void print_temperature_data(SensorData data, const char *client_ip) {
    char *time_str = get_timestamp_string(data.timestamp);
    
    printf("%s - Sensor #%d (%s): %d°C\n", 
           time_str, 
           data.id,
           client_ip, 
           data.temperature);
    
    free(time_str);
    
    // Add warning for extreme temperatures
    if (data.temperature > 35) {
        printf("  ⚠️ WARNING: High temperature detected!\n");
    } else if (data.temperature < 15) {
        printf("  ❄️ NOTICE: Low temperature detected.\n");
    }
    
    // Flush stdout to ensure immediate display
    fflush(stdout);
}

