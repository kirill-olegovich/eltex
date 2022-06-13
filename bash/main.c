#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    char command[100];
    pid_t pid;

    while (1) {
        int fd[2];
        pipe(fd);

        printf("# ");
        memset(command, 0, 100);
        fgets(command, 100, stdin);
        command[strlen(command) - 1] = '\0';

        if (strcmp(command, "exit") == 0)
            break;

        pid = fork();

        if (pid == 0) {
            int pipe_index = -1, save_fd1;
            char temp_str1[100];

            dup2(fd[0], 0);
            close(fd[0]);

            do {
                int i;

                for (i = 0; command[i] != '\0'; i++) {
                    if (command[i] == '|') {
                        pipe_index = i;
                        break;
                    }
                }

                if (pipe_index == -1)
                    break;

                memset(temp_str1, 0, 100);

                for (i = 0; i < pipe_index; i++)
                    temp_str1[i] = command[i];
                temp_str1[pipe_index] = '\0';

                for (i = 0; command[i + pipe_index + 1] != '\0'; i++)
                    command[i] = command[i + pipe_index + 1];
                command[i] = '\0';

                save_fd1 = dup(1);

                dup2(fd[1], 1);

                system(temp_str1);

                dup2(save_fd1, 1);
                close(save_fd1);

                // char temp[4096];
                // read(fd[0], temp, 4096);
                // printf("%s\n", temp);

                pipe_index = -1;

            } while (1);

            system(command);
            exit(0);

        } else if (pid > 0) {
            wait(NULL);

        } else
            printf("Error\n");
    }

    return 0;
}
