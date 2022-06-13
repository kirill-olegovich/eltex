#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    pid_t pid;
    pid = fork();

    if (pid == 0) {
        struct mq_attr attr;
        mqd_t mqid;

        memset(&attr, 0, sizeof attr);
        attr.mq_msgsize = 4096;
        attr.mq_maxmsg = 10;

        mqid = mq_open("/buffer", O_CREAT | O_WRONLY, 0600, &attr);

        if (mqid == -1) {
            perror("mq_open");
            exit(1);
        }

        mq_send(mqid, "qweasdzxc", 9, 0);
        mq_close(mqid);

        exit(0);

    } else if (pid > 0) {
        int child_status;
        waitpid(pid, &child_status, 0);

        if (WEXITSTATUS(child_status) == 1)
            exit(1);

        struct mq_attr attr;
        unsigned int prior = 0;
        mqd_t mqid;

        mqid = mq_open("/buffer", O_RDONLY);

        if (mqid == -1) {
            perror("mq_open");
            exit(1);
        }

        mq_getattr(mqid, &attr);
        char buffer[attr.mq_msgsize];

        if ((mq_receive(mqid, buffer, attr.mq_msgsize, &prior)) == -1) {
            perror("mq_receive");
            exit(1);
        }

        printf("%s %ld\n", buffer, attr.mq_msgsize);

        mq_close(mqid);
        mq_unlink("/buffer");

    } else
        printf("Error\n");

    return 0;
}