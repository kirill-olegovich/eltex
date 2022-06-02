#include <unistd.h>
#include <stdio.h>
#include <signal.h>

int main(void) {
	int num;
	scanf("%d", &num);
	kill(num, 15);
	
	return 0;
}
