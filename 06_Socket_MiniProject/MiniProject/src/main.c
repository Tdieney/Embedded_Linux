#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netdb.h>

#define MAX_CONNECTIONS 10U
#define MAX_MESSAGE_LEN 100U
#define BUFFER_SIZE     128U

// Structure to store connection information
typedef struct {
    int id;
    int socket_fd;
    char ip[INET_ADDRSTRLEN];
    int port;
    pthread_t thread;
    int active;
} Connection;

// Global variables
Connection connections[MAX_CONNECTIONS];
int connection_count = 0;
int listen_port = 12345; // Default listening port
pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function declarations
void *server_thread(void *arg);
void *connection_handler(void *arg);
void handle_command(char *command);
int validate_ip(const char *ip);
void cleanup_connection(int id);
char *get_local_ip();
void display_message(const char *ip, int port, const char *message);

int main(int argc, char *argv[]) {
    if (argc > 1) {
        listen_port = atoi(argv[1]);
    } else {
        printf("Usage: %s <port>\n", argv[0]);
        printf("Using default port %d\n", listen_port);
    }

    // Initialize connections array
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        connections[i].active = 0;
    }

    // Start server thread to listen for incoming connections
    pthread_t server_tid;
    if (pthread_create(&server_tid, NULL, server_thread, NULL) != 0) {
        printf("Failed to start server thread");
        exit(EXIT_FAILURE);
    }

    // Main loop for CLI
    char command[BUFFER_SIZE];
    while (1) {
        printf("> ");
        fgets(command, BUFFER_SIZE, stdin);
        command[strcspn(command, "\n")] = 0; // Remove newline
        handle_command(command);
    }

    return 0;
}

void *server_thread(void *arg) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        printf("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Enable SO_REUSEADDR to allow port reuse
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        printf("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(listen_port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        printf("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", listen_port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            printf("Accept failed");
            continue;
        }

        // Add new connection to the list
        pthread_mutex_lock(&connection_mutex);
        if (connection_count >= MAX_CONNECTIONS) {
            printf("Error: Maximum connections reached.\n");
            close(client_fd);
            pthread_mutex_unlock(&connection_mutex);
            continue;
        }

        int id = connection_count++;
        connections[id].id = id;
        connections[id].socket_fd = client_fd;
        inet_ntop(AF_INET, &client_addr.sin_addr, connections[id].ip, INET_ADDRSTRLEN);
        connections[id].port = ntohs(client_addr.sin_port);
        connections[id].active = 1;
        pthread_mutex_unlock(&connection_mutex);

        printf("New connection from %s:%d (ID: %d)\n", connections[id].ip, connections[id].port, id);

        // Start a thread to handle this connection
        pthread_create(&connections[id].thread, NULL, connection_handler, &connections[id]);
    }

    close(server_fd);
    return NULL;
}

void *connection_handler(void *arg) {
    Connection *conn = (Connection *)arg;
    char buffer[BUFFER_SIZE];
    int client_fd = conn->socket_fd;

    while (1) {
        int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("Connection with %s:%d (ID: %d) lost.\n", conn->ip, conn->port, conn->id);
            pthread_mutex_lock(&connection_mutex);
            cleanup_connection(conn->id);
            pthread_mutex_unlock(&connection_mutex);
            break;
        }

        buffer[bytes_received] = '\0';
        display_message(conn->ip, conn->port, buffer);
    }

    return NULL;
}

void display_message(const char *ip, int port, const char *message) {
    printf("\nMessage from %s:%d: %s\n> ", ip, port, message);
    fflush(stdout);
}

void handle_command(char *command) {
    char *token = strtok(command, " ");
    if (!token) return;

    if (strcmp(token, "help") == 0) {
        printf("help: Display usage instructions\n");
        printf("myip: Display the IP address of this machine\n");
        printf("myport: Display the port this program is listening on\n");
        printf("connect <destination> <port>: Connect to a peer at <destination> IP and <port>\n");
        printf("list: List all active connections\n");
        printf("terminate <connection id>: Terminate a connection by ID\n");
        printf("send <connection id> <message>: Send a message to a peer by ID\n");
        printf("exit: Close all connections and exit the program\n");
    }
    else if (strcmp(token, "myip") == 0) {
        char *ip = get_local_ip();
        if (ip) {
            printf("IP Address: %s\n", ip);
            free(ip);
        } else {
            printf("Error: Could not retrieve IP address.\n");
        }
    }
    else if (strcmp(token, "myport") == 0) {
        printf("Listening on port: %d\n", listen_port);
    }
    else if (strcmp(token, "connect") == 0) {
        char *ip = strtok(NULL, " ");
        char *port_str = strtok(NULL, " ");
        if (!ip || !port_str) {
            printf("Usage: connect <destination> <port>\n");
            return;
        }

        int port = atoi(port_str);
        if (!validate_ip(ip)) {
            printf("Error: Invalid IP address.\n");
            return;
        }

        // Check for self-connection
        char *local_ip = get_local_ip();
        if (local_ip && strcmp(ip, local_ip) == 0 && port == listen_port) {
            printf("Error: Cannot connect to self.\n");
            free(local_ip);
            return;
        }
        free(local_ip);

        // Check for duplicate connection
        pthread_mutex_lock(&connection_mutex);
        for (int i = 0; i < connection_count; i++) {
            if (connections[i].active && strcmp(connections[i].ip, ip) == 0 && connections[i].port == port) {
                printf("Error: Connection already exists.\n");
                pthread_mutex_unlock(&connection_mutex);
                return;
            }
        }

        if (connection_count >= MAX_CONNECTIONS) {
            printf("Error: Maximum connections reached.\n");
            pthread_mutex_unlock(&connection_mutex);
            return;
        }
        pthread_mutex_unlock(&connection_mutex);

        // Create a new connection
        int client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd < 0) {
            printf("Socket creation failed");
            return;
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, ip, &server_addr.sin_addr);

        if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            printf("Connection failed");
            close(client_fd);
            return;
        }

        pthread_mutex_lock(&connection_mutex);
        int id = connection_count++;
        connections[id].id = id;
        connections[id].socket_fd = client_fd;
        strcpy(connections[id].ip, ip);
        connections[id].port = port;
        connections[id].active = 1;
        pthread_mutex_unlock(&connection_mutex);

        printf("Successfully connected to %s:%d (ID: %d)\n", ip, port, id);

        // Start a thread to handle this connection
        pthread_create(&connections[id].thread, NULL, connection_handler, &connections[id]);
    }
    else if (strcmp(token, "list") == 0) {
        printf("ID | IP Address    | Port\n");
        pthread_mutex_lock(&connection_mutex);
        for (int i = 0; i < connection_count; i++) {
            if (connections[i].active) {
                printf("%d  | %s | %d\n", connections[i].id, connections[i].ip, connections[i].port);
            }
        }
        pthread_mutex_unlock(&connection_mutex);
    }
    else if (strcmp(token, "terminate") == 0) {
        char *id_str = strtok(NULL, " ");
        if (!id_str) {
            printf("Usage: terminate <connection id>\n");
            return;
        }

        int id = atoi(id_str);
        pthread_mutex_lock(&connection_mutex);
        if (id < 0 || id >= connection_count || !connections[id].active) {
            printf("Error: Invalid connection ID.\n");
            pthread_mutex_unlock(&connection_mutex);
            return;
        }

        cleanup_connection(id);
        printf("Connection with ID %d terminated.\n", id);
        pthread_mutex_unlock(&connection_mutex);
    }
    else if (strcmp(token, "send") == 0) {
        char *id_str = strtok(NULL, " ");
        char *message = strtok(NULL, "");
        if (!id_str || !message) {
            printf("Usage: send <connection id> <message>\n");
            return;
        }

        int id = atoi(id_str);
        pthread_mutex_lock(&connection_mutex);
        if (id < 0 || id >= connection_count || !connections[id].active) {
            printf("Error: Invalid connection ID.\n");
            pthread_mutex_unlock(&connection_mutex);
            return;
        }

        if (strlen(message) > MAX_MESSAGE_LEN) {
            printf("Error: Message exceeds 100 characters.\n");
            pthread_mutex_unlock(&connection_mutex);
            return;
        }

        int client_fd = connections[id].socket_fd;
        if (send(client_fd, message, strlen(message) + 1, 0) < 0) {
            printf("Send failed");
            cleanup_connection(id);
        } else {
            printf("Message sent successfully to ID %d.\n", id);
        }
        pthread_mutex_unlock(&connection_mutex);
    }
    else if (strcmp(token, "exit") == 0) {
        pthread_mutex_lock(&connection_mutex);
        for (int i = 0; i < connection_count; i++) {
            if (connections[i].active) {
                cleanup_connection(i);
            }
        }
        pthread_mutex_unlock(&connection_mutex);
        printf("All connections closed. Exiting program.\n");
        exit(EXIT_SUCCESS);
    }
    else {
        printf("Unknown command. Type 'help' for usage instructions.\n");
    }
}

int validate_ip(const char *ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) != 0;
}

void cleanup_connection(int id) {
    if (connections[id].active) {
        close(connections[id].socket_fd);
        connections[id].active = 0;
        pthread_cancel(connections[id].thread);
    }
}

char *get_local_ip() {
    struct ifaddrs *ifaddr, *ifa;
    char *ip = NULL;

    if (getifaddrs(&ifaddr) == -1) {
        printf("getifaddrs failed");
        return NULL;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        if (ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, "lo") != 0) {
            ip = malloc(INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr, ip, INET_ADDRSTRLEN);
            break;
        }
    }

    freeifaddrs(ifaddr);
    return ip;
}
