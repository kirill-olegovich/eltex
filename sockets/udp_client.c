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
	// char *hello = "Hello from client"; 

	if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	memset(&serveraddr, 0, sizeof(serveraddr)); 

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8888);
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	
	length = sizeof(serveraddr);

	while (1) {
		bzero(buffer, 1024);
		
		printf("Message to server: ");

		fgets(buffer, 1024, stdin);

		sendto(socketfd, buffer, sizeof(buffer), MSG_CONFIRM, (struct sockaddr*)&serveraddr, length);

		if (strncmp(buffer, "exit", 4) == 0) break;		

		recvfrom(socketfd, buffer, 1024, MSG_WAITALL, (struct sockaddr*)&serveraddr, &length);
	
		printf("\nFrom server: %s\n", buffer);
	}

	close(socketfd);

	return 0;
}
