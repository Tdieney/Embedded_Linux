#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8088
#define LISTEN_BACKLOG 50

int main(int argc, char *argv[]) {
    int port_no, len, opt;
    int server_fd, new_socket_fd;
    struct sockaddr_in serv_addr, client_addr;
    char buffer[256] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    /* Ngăn lồi : "address already in use" */
    (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        printf("Bind failed\n");

    if (listen(server_fd, LISTEN_BACKLOG) == -1)
        printf("Listen failed");

    new_socket_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&len);
    if (new_socket_fd == -1)
        printf("accept failed");

    read(new_socket_fd, buffer, sizeof(buffer));
    printf("Message recevied: %s\n", buffer);

    return 0;
}