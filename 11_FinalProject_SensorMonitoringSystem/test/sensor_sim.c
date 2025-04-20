#include <sensor_lib.h>
#include <signal.h>
#include <stdbool.h>

volatile sig_atomic_t running = true;

// Signal handler for SIGTERM
void handle_sigterm(int sig) {
    running = false;
}

int main(int argc, char *argv[]) {
    // Validate command line arguments
    if (argc != THREE) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint16_t port = atoi(argv[ONE]);
    uint16_t ss_id = atoi(argv[TWO]);
    int client_sock;
    SensorData temp_data;
    
    // Register signals handler
    signal(SIGTERM, handle_sigterm);
    signal(SIGINT, handle_sigterm);
    
    // Create socket
    client_sock = create_socket();
    if (client_sock < 0) {
        return EXIT_FAILURE;
    }
    
    // Setup connection to server
    if (setup_server_connection(client_sock, SERVER_IP, port) < 0) {
        close(client_sock);
        return EXIT_FAILURE;
    }
    
    printf("Sensor connected to gateway on port %d\n", port);
    
    // Initialize the randomizer
    srand(time(NULL));
    
    // Main data transmission loop
    while (running) {
        temp_data = generate_sensor_data();
        temp_data.id = ss_id;
        
        if (send_temperature_data(client_sock, temp_data) < 0) {
            break;
        }
        
        print_temp_data(temp_data);
        
        // Sleep for interval
        usleep(TIMER_100MS);
    }
    
    close(client_sock);
    return EXIT_SUCCESS;
}
