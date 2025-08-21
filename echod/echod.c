#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 12345

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;

    // Create a TCP (SOCK_STREAM) socket in the IPv4 (AF_INET) domain
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("Error opening socket");
        exit(0);
    }

    // Zero out the address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;                 // IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;         // Listen on all interfaces
    serv_addr.sin_port = htons(PORT);    // Port in network byte order (htons)

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error on binding");
        close(sockfd);
        exit(1);
    }

    printf("Socked bound to port %d\n", ntohs(serv_addr.sin_port));
    close(sockfd);

    return 0;
}
