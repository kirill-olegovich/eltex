#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int max(int x, int y) {
    if (x > y)
        return x;
    else
        return y;
}

int main(void) {
    int socketfd_tcp, socketfd_udp, length, connectedfd, max_val, ready;
    struct sockaddr_in serveraddr, clientaddr;
    pid_t pid;
    fd_set rset;

    if ((socketfd_tcp = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if ((socketfd_udp = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8888);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socketfd_tcp, (struct sockaddr *)&serveraddr,
             sizeof(serveraddr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (bind(socketfd_udp, (struct sockaddr *)&serveraddr,
             sizeof(serveraddr)) == -1) {
        perror("bind");
        exit(1);
    }

    listen(socketfd_tcp, 5);

    max_val = max(socketfd_tcp, socketfd_udp) + 1;

    while (1) {
        FD_ZERO(&rset);
        FD_SET(socketfd_tcp, &rset);
        FD_SET(socketfd_udp, &rset);

        ready = select(max_val, &rset, NULL, NULL, NULL);

        if (FD_ISSET(socketfd_tcp, &rset)) {
            length = sizeof(clientaddr);
            connectedfd =
                accept(socketfd_tcp, (struct sockaddr *)&clientaddr, &length);

            char buffer[1024];
            bzero(buffer, 1024);
            read(connectedfd, buffer, 1024);
            printf("From client (%d): %s\n", connectedfd, buffer);
            write(connectedfd, "OK", 2);
            sleep(2);

            close(connectedfd);
        }

        else if (FD_ISSET(socketfd_udp, &rset)) {
            length = sizeof(clientaddr);

            char buffer[1024];
            bzero(buffer, 1024);
            recvfrom(socketfd_udp, buffer, 1024, MSG_WAITALL,
                     (struct sockaddr *)&clientaddr, &length);
            printf("From client (%d): %s\n", connectedfd, buffer);
            sendto(socketfd_udp, "OK", 2, MSG_CONFIRM,
                   (struct sockaddr *)&clientaddr, length);
            sleep(2);

            close(connectedfd);
        }
    }

    return 0;
}
