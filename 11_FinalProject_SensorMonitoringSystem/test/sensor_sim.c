#include <sensor_lib.h>

uint8 main(int argc, char *argv[]) {
    // Validate command line arguments
    if (argc != THREE) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint16 port = atoi(argv[ONE]);
    uint16 ss_id = atoi(argv[TWO]);
    int client_sock;
    SensorData temp_data;
    
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
    while (ONE) {
        temp_data = generate_sensor_data();
        temp_data.id = ss_id;
        
        if (send_temperature_data(client_sock, temp_data) < 0) {
            break;
        }
        
        print_temp_data(temp_data);
        
        // Sleep for interval
        sleep(ONE);
    }
    
    close(client_sock);
    return EXIT_SUCCESS;
}
