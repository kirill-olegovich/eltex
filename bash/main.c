#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void) {
	char command[10];
	pid_t pid;

	while (1) {
		printf("# ");
		fgets(command, 10, stdin);
		command[strlen(command) - 1] = '\0';
		
		if (strcmp(command, "exit") == 0) break;

		pid = fork();

		if (pid == 0) {
			system(command);
			exit(0);
		} else if (pid > 0) {
			wait(NULL);
		} else printf("Error\n");

	}
	
	return 0;
}
