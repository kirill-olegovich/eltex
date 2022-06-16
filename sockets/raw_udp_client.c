#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <unistd.h>


int main(void) {
    int sock, recv;
    socklen_t length;
    char message[] = "HI";
    char buffer[1024];
    char rec_buffer[1024];
    struct sockaddr_in ip;
    struct udphdr udp;

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

    ip.sin_family = AF_INET;
    ip.sin_addr.s_addr = inet_addr("127.0.0.1");

    udp.source = htons(9999);
    udp.dest = htons(8888);
    // udp.check = 0;
    udp.len = htons(sizeof(struct udphdr) + strlen(message));
    
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, (char *)&udp, sizeof(udp));
    memcpy(buffer + sizeof(udp), (char *)&message, sizeof(message));

    length = sizeof(ip);

    for (;;) {
        sleep(1);
        
        sendto(sock, buffer, sizeof(udp) + sizeof(message), 0, (struct sockaddr *)&ip, sizeof(ip));

        memset(rec_buffer, 0, sizeof(rec_buffer));

        recv = recvfrom(sock, (char *)&rec_buffer, sizeof(rec_buffer), 0, (struct sockaddr *)&ip, &length);

        // if (8888 == htons(ip.sin_port)) {
        //     printf("YES\n");
        //     break;
        // }

        printf("%d\n", htons(ip.sin_port));
    }

    close(sock);

    return 0;
}
