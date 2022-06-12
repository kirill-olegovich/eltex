#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(void) {
	int socketfd;
	struct sockaddr_in serveraddr, clientaddr;
	char buffer[1024];

	if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8888);
	serveraddr.sin_addr.s_addr = inet_addr("192.168.1.36");

	connect(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

	while (1) {
		bzero(buffer, 1024);

		printf("Message to server: ");

		fgets(buffer, 1024, stdin);

		write(socketfd, buffer, 1024);

		if (strncmp(buffer, "exit", 4) == 0) break;

		read(socketfd, buffer, 1024);

		printf("\nFrom server: %s\n", buffer);
	}

	close(socketfd);

	return 0;
}
