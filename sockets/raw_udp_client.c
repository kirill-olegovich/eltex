#include <errno.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void) {
    int sock, recv;
    socklen_t length;
    char message[] = "HI";
    char buffer[1024];
    char rec_buffer[1024];
    struct sockaddr_in ip;
    struct udphdr udp, rec_udp;

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

    ip.sin_family = AF_INET;
    ip.sin_addr.s_addr = inet_addr("127.0.0.1");

    udp.source = htons(9999);
    udp.dest = htons(8888);
    udp.check = 0;
    udp.len = htons(sizeof(struct udphdr) + strlen(message));

    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, (char *)&udp, sizeof(udp));
    memcpy(buffer + sizeof(udp), (char *)&message, sizeof(message));

    length = sizeof(ip);

    for (;;) {
        sleep(1);

        sendto(sock, buffer, sizeof(udp) + sizeof(message), 0,
               (struct sockaddr *)&ip, sizeof(ip));

        memset(rec_buffer, 0, sizeof(rec_buffer));
        memset((struct udphdr *)&rec_udp, 0, sizeof(rec_udp));

        recv = recvfrom(sock, (char *)&rec_buffer, sizeof(rec_buffer), 0,
                        (struct sockaddr *)&ip, &length);

        memcpy((struct udphdr *)&rec_udp, (struct udphdr *)&rec_buffer[20], 8);

        if (8888 == htons(rec_udp.source)) {
            printf("%s\n", rec_buffer + 28);
            break;
        }

        printf("%d\n", htons(rec_udp.source));
    }

    close(sock);

    return 0;
}
