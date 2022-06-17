#include <errno.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void) {
    int sock;
    socklen_t length;
    char *message;
    char packet[1024];
    char rec_buffer[1024];

    memset(packet, 0, 1024);

    struct udphdr *udp = (struct udphdr *)packet;
    struct sockaddr_in sin;
    struct udphdr rec_udp;

    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
        perror("sock");
        exit(1);
    }

    message = packet + sizeof(struct udphdr);
    strcpy(message, "HI");

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");

    udp->source = htons(9999);
    udp->dest = htons(8888);
    udp->check = 0;
    udp->len = htons(sizeof(struct udphdr) + strlen(message));

    length = sizeof(struct sockaddr_in);

    if (sendto(sock, packet, sizeof(struct udphdr) + sizeof(message), 0,
               (struct sockaddr *)&sin, length) < 0) {
        perror("sendto");
        close(sock);
        exit(1);
    }

    for (;;) {
        sleep(1);

        memset(rec_buffer, 0, sizeof(rec_buffer));
        memset((struct udphdr *)&rec_udp, 0, sizeof(rec_udp));

        recvfrom(sock, (char *)&rec_buffer, sizeof(rec_buffer), 0,
                 (struct sockaddr *)&sin, &length);

        memcpy((struct udphdr *)&rec_udp, (struct udphdr *)&rec_buffer[20], 8);

        if (8888 == htons(rec_udp.source)) {
            printf("%s\n", rec_buffer + 28);
            break;
        }

        // printf("%d\n", htons(rec_udp.source));
    }

    close(sock);

    return 0;
}
