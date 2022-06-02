#include <unistd.h>
#include <stdio.h>
#include <signal.h>

void handler(int num) {
	printf("Number of signal is %d\n", num);
}

int main(void) {
	signal(15, handler);
	
	while (1) {
		printf("Waiting for signal...\n");
		sleep(1);
	}
	
	return 0;
}
