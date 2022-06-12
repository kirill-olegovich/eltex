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
	struct sockaddr_in serveraddr, clientaddr;
	char buffer[1024];

	if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	memset(&serveraddr, 0, sizeof(serveraddr)); 
    memset(&clientaddr, 0, sizeof(clientaddr)); 

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8888);
	serveraddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
		perror("bind");
		exit(1);
	}
	
	length = sizeof(clientaddr);

	while (1) {
		bzero(buffer, 1024);

		recvfrom(socketfd, buffer, 1024, MSG_WAITALL, (struct sockaddr*)&clientaddr, &length);

		printf("\nFrom client: %s\nMessage to client: ", buffer);
		
		fgets(buffer, 1024, stdin);

		sendto(socketfd, buffer, 1024, MSG_CONFIRM, (struct sockaddr*)&clientaddr, length);

		if (strncmp(buffer, "exit", 4) == 0) break;
	}

	close(socketfd);

	return 0;
}
