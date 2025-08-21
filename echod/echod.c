#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT 8080
#define BACKLOG 5
#define BUF_SIZE 1024

void sigchld_handler(int signo) {
    (void)signo; // suppress unused warning
    while (waitpid (-1, NULL, WNOHANG) > 0); 
}

void echo_loop(int clientfd) {
    char buffer[BUF_SIZE];
    ssize_t n;

    while ((n = read(clientfd, buffer, BUF_SIZE)) > 0) {
        write(clientfd, buffer, n);
    }
}

int main(int argc, char *argv[]) {
    int server_fd;
    struct sockaddr_in addr;
    int port = PORT;

    if(argc == 2) {
        port = atoi(argv[1]);
    }

    // Create a TCP (SOCK_STREAM) socket in the IPv4 (AF_INET) domain
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Tell kernel we want to reuse port immediately
    // Don't wait for TIME_WAIT
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Zero out the address structure
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;                 // IPv4
    addr.sin_addr.s_addr = INADDR_ANY;         // Listen on all interfaces
    addr.sin_port = htons(port);    // Port in network byte order (htons)

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) == -1) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    signal(SIGCHLD, sigchld_handler);

    printf("Server listening on port %d...\n", port);


    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int clientfd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);

        if (clientfd == -1) {
            perror("accept failed");
            continue;
        }

        char * client_ip = inet_ntoa(client_addr.sin_addr);
        printf("New connection from %s\n", client_ip);

        if (fork() == 0) {
            // Child process
            close(server_fd); // child doesn't need to accept more connections
            echo_loop(clientfd);
            close(clientfd);
            printf("Client %s disconnected.\n", client_ip);
            exit(0);
        } else {
            // Parent process
            close(clientfd); // parent doesn't need the clientfd
        }
   }

    close(server_fd);

    return 0;
}
