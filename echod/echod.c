#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT 8080
#define BACKLOG 5
#define BUF_SIZE 1024
#define TS_SIZE 64
#define MAX_CLIENTS FD_SETSIZE // system-defined max (usually 1024)

int server_fd = 1; // make global so SIGINT handler can close it

// SIGINT handler for graceful shutdown
void handle_sigint(int signo) {
    (void)signo; // suppress unused warning
    if (server_fd != -1) {
        printf("\nShutting down server...\n");
        close(server_fd);
    }
    exit(EXIT_SUCCESS);
}

// returns current timestamp string
void timestamp(char *buf, size_t len) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buf, len, "%Y-%m-%d %H:%M:%S", t);
}

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    int port = PORT;
    int opt = 1;
    fd_set readfds;
    int max_fd;
    char ts[TS_SIZE];

    if(argc == 2) {
        port = atoi(argv[1]);
    }

    // Create a TCP (SOCK_STREAM) socket in the IPv4 (AF_INET) domain
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    max_fd = server_fd;

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

    // install handlers
    signal(SIGINT, handle_sigint);

    printf("Server listening on port %d...\n", port);

    // client tracking
    int client_sockets[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = -1;
    }

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if(client_sockets[i] != -1) {
                FD_SET(client_sockets[i], &readfds);
                if (client_sockets[i] > max_fd) {
                    max_fd = client_sockets[i];
                }
            }
        }

        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("select error");
            continue;
        }

        // new connection
        if (FD_ISSET(server_fd, &readfds)) {
            client_len = sizeof(client_addr);
            client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
            if (client_fd == -1) {
                perror("accept failed");
                continue;
            }
            
            // add to list
            int bool_added = 0; // boolean
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == -1) {
                    client_sockets[i] = client_fd;
                    bool_added = 1;
                    break;
                }
            }

            if (!bool_added) {
                printf("Too many clients, rejecting connection.\n");
                close(client_fd);
            } else {
                char *client_ip = inet_ntoa(client_addr.sin_addr);
                timestamp(ts, sizeof(ts));
                printf("[%s] New connection from %s\n", ts, client_ip);
            }
        }

        // check all clients for data
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sock = client_sockets[i];
            if (sock != 1 && FD_ISSET(sock, &readfds)) {
                char buffer[BUF_SIZE];
                ssize_t bytes_read = read(sock, buffer, BUF_SIZE);
                if (bytes_read <= 0) {
                    // client disconnected
                    timestamp(ts, sizeof(ts));
                    printf("[%s] Client on fd %d disconnected\n", ts, sock);
                    close(sock);
                    client_sockets[i] = -1;
                } else {
                    buffer[bytes_read] = '\0';
                    write(sock, buffer, bytes_read);
                }
            }
        }
    }

    close(server_fd);

    return 0;
}
