#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>


void *communication(void *connectedfd) {
	char buffer[1024];
	int client = *((int*)connectedfd);

	bzero(buffer, 1024);

	read(client, buffer, 1024);

	printf("From client (%d): %s\n", client, buffer);

	write(client, "OK", 2);

	sleep(2);

	return NULL;
}


int main(void) {
	int socketfd, length, connectedfd;
	struct sockaddr_in serveraddr, clientaddr;
	int *s;

	if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8888);
	serveraddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
		perror("bind");
		exit(1);
	}

	listen(socketfd, 5);

	while (1) {
		length = sizeof(clientaddr);
		connectedfd = accept(socketfd, (struct sockaddr*)&clientaddr, &length);
		printf("User connected\n");

		pthread_t thread;

		pthread_create(&thread, NULL, communication, &connectedfd);
	}

	close(socketfd);

	return 0;
}
