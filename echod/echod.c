#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define BACKLOG 5
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd;
    int clientfd;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUF_SIZE];
    int port = PORT;

    if(argc == 2) {
        port = atoi(argv[1]);
    }

    // Create a TCP (SOCK_STREAM) socket in the IPv4 (AF_INET) domain
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Zero out the address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;                 // IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;         // Listen on all interfaces
    serv_addr.sin_port = htons(port);    // Port in network byte order (htons)

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, BACKLOG) < 0) {
        perror("listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    clientfd = accept(sockfd, (struct sockaddr*) &client_addr, &client_addr_len);
    if (clientfd < 0) {
        perror("accept failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Client connected!\n");

    // Echo loop
    while (1) {
        ssize_t bytes_read = read(clientfd, buffer, BUF_SIZE - 1);
        if (bytes_read <= 0) {
            printf("Client disconnected\n");
            break;
        }
        buffer[bytes_read] = '\0';
        printf("Received %ld: %s", bytes_read, buffer);

        // echo back
        if (write(clientfd, buffer, bytes_read) < 0) {
            perror("write failed");
            break;
        }
    }

    close(clientfd);
    close(sockfd);

    return 0;
}
