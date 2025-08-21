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

// SIGCHLD handler to reap zombie processes
void reap_children(int signo) {
    (void)signo; // suppress unused warning
    while (waitpid (-1, NULL, WNOHANG) > 0) {
        // Clean up all terminated children
    }; 
}

void echo_loop(int clientfd) {
    char buffer[BUF_SIZE];
    while(1) {
        ssize_t bytes_read = read(clientfd, buffer, BUF_SIZE);
        if( bytes_read < 0) {
            break; // client closed or error
        }
        buffer[bytes_read] = '\0';
        write(clientfd, buffer, bytes_read);
    }
}

int main(int argc, char *argv[]) {
    int server_fd;
    int client_fd;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    int port = PORT;
    int opt = 1;

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
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Zero out the address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;                 // IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;         // Listen on all interfaces
    serv_addr.sin_port = htons(port);    // Port in network byte order (htons)

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) == -1) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // install handler for zombie cleanup
    signal(SIGCHLD, reap_children);

    printf("Server listening on port %d...\n", port);


    while (1) {
        client_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

        if (client_fd == -1) {
            perror("accept failed");
            continue;
        }

        char * client_ip = inet_ntoa(client_addr.sin_addr);
        printf("New connection from %s\n", client_ip);

        pid_t pid = fork();
        if(pid < 0) {
            perror("fork failed");
            close(client_fd);
            continue;
        }

        if (pid == 0) {
            // Child process
            close(server_fd); // child doesn't need to accept more connections
            echo_loop(client_fd);
            close(client_fd);
            printf("Client %s disconnected.\n", client_ip);
            exit(EXIT_SUCCESS);
        } else {
            // Parent process
            close(client_fd); // parent doesn't need the clientfd
        }
   }

    close(server_fd);

    return 0;
}
