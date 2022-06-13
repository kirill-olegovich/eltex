#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

int main(void) {
	int socketfd, length;
	struct sockaddr_in serveraddr;
	char buffer[1024];

	if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	memset(&serveraddr, 0, sizeof(serveraddr)); 

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8888);
	serveraddr.sin_addr.s_addr = INADDR_ANY;

	length = sizeof(serveraddr);

	bzero(buffer, 1024);
	
	sendto(socketfd, "HI", 2, MSG_CONFIRM, (struct sockaddr*)&serveraddr, length);

	recvfrom(socketfd, buffer, 1024, MSG_WAITALL, (struct sockaddr*)&serveraddr, &length);

	printf("From server: %s\n", buffer);

	close(socketfd);

	return 0;
}
