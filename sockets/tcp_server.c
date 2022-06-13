#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(void) {
    int socketfd, length, connectedfd;
    struct sockaddr_in serveraddr, clientaddr;
    char buffer[1024];

    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8888);
    serveraddr.sin_addr.s_addr = inet_addr("192.168.1.36");

    if (bind(socketfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) ==
        -1) {
        perror("bind");
        exit(1);
    }

    listen(socketfd, 5);

    length = sizeof(clientaddr);

    connectedfd = accept(socketfd, (struct sockaddr *)&clientaddr, &length);

    while (1) {
        bzero(buffer, 1024);

        read(connectedfd, buffer, 1024);

        printf("\nFrom client: %s\nMessage to client: ", buffer);

        fgets(buffer, 1024, stdin);

        write(connectedfd, buffer, 1024);

        if (strncmp(buffer, "exit", 4) == 0)
            break;
    }

    close(socketfd);

    return 0;
}
