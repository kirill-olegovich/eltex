#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
    int num;
    scanf("%d", &num);
    kill(num, 15);

    return 0;
}
