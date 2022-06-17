#include <errno.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


unsigned short csum(unsigned short *ptr,int nbytes) 
{
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
    
    return(answer);
}


struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};


int main(void) {
    int sock, length, one = 1;
    char *message;
    char packet[1024];
    char rec_buffer[1024];

    memset(packet, 0, 1024);

    struct iphdr *ip = (struct iphdr *)packet;
    struct udphdr *udp = (struct udphdr *)(packet + sizeof(struct iphdr));
    struct sockaddr_in sin;
    struct pseudo_header psh;
    struct iphdr rec_ip;
    struct udphdr rec_udp;

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    message = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(message, "HI");

    sin.sin_family = AF_INET;
    // sin.sin_port = htons(123);
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");

    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(message);
    ip->id = htonl(54321);
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = 17;
    // ip->check = 0;
    ip->saddr = inet_addr("127.0.0.1");
    ip->daddr = inet_addr("127.0.0.1");
    ip->check = csum((unsigned short *)packet, ip->tot_len);

    udp->source = htons(9999);
    udp->dest = htons(8888);
    udp->len = htons(sizeof(struct udphdr) + strlen(message));
    udp->check = 0;

    psh.source_address = inet_addr("127.0.0.1");
    psh.dest_address = inet_addr("127.0.0.1");
    psh.placeholder = 0;
    psh.protocol = 17;
    psh.udp_length = htons(sizeof(struct udphdr) + strlen(message));

    int psize = sizeof(struct pseudo_header) + sizeof(struct udphdr) + strlen(message); 
    char pseudopacket[psize];

    memset(pseudopacket, 0, psize);

    memcpy(pseudopacket, (char *)&psh, sizeof(struct pseudo_header));
    memcpy(pseudopacket + sizeof(struct pseudo_header) , udp , sizeof(struct udphdr) + strlen(message));

    udp->check = csum((unsigned short *)pseudopacket, psize);

    printf("IP checksum:    %d\n", ip->check);
    printf("UDP checksum:   %d\n", udp->check);

    length = sizeof(struct sockaddr_in);

    if (sendto(sock, packet, ip->tot_len, 0, (struct sockaddr *)&sin, length) < 0) {
            perror("sendto");
            exit(1);
    }

    for (;;) {
        sleep(1);
 
        memset(rec_buffer, 0, sizeof(rec_buffer));
        memset((struct iphdr *)&rec_ip, 0, sizeof(rec_ip));
        memset((struct udphdr *)&rec_udp, 0, sizeof(rec_udp));

        if (recvfrom(sock, (char *)&rec_buffer, sizeof(rec_buffer), 0, (struct sockaddr *)&sin, &length) < 0) {
            perror("recvfrom");
        }

        memcpy((struct iphdr *)&rec_ip, (struct iphdr *)&rec_buffer, 20);
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
