#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8088

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in serv_addr;
    const char *message = "Hello from client";

    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
        printf("connect failed\n");

    write(client_fd, message, strlen(message) + 1);

    return 0;
}