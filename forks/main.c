#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int p_id, c_id, p_id_temp;
    pid_t pid;

    c_id = getpid();
    p_id = getppid();
    printf("Main Process: Child id %d Parent id %d\n", c_id, p_id);

    pid = fork();

    if (pid == 0) {
        c_id = getpid();
        p_id = getppid();
        printf("Child id %d Parent id %d\n", c_id, p_id);

        pid = fork();

        if (pid == 0) {
            c_id = getpid();
            p_id = getppid();
            printf("Child id %d Parent id %d\n", c_id, p_id);
        }

        // printf("CHILD\n");

    } else {
        pid = fork();

        if (pid == 0) {
            c_id = getpid();
            p_id = getppid();
            printf("Child id %d Parent id %d\n", c_id, p_id);

            pid = fork();
            // printf("%d\n", pid);

            if (pid != 0) {
                pid = fork();

                if (pid == 0) {
                    c_id = getpid();
                    p_id = getppid();
                    printf("Child id %d Parent id %d\n", c_id, p_id);
                }

            } else {
                c_id = getpid();
                p_id = getppid();
                printf("Child id %d Parent id %d\n", c_id, p_id);
            }
        }

        // printf("PARENT\n");
    }

    sleep(10);

    return 0;
}
