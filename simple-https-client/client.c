#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {
        AF_INET,
        htons(443),
        htonl(0x08080808)
    };
    connect(sockfd, (const struct *sockaddr)&addr, sizeof(addr));

    return 0;
}
