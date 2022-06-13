#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct msgbuf {
    long mtype;
    char mtext[100];
};

int main(void) {
    pid_t pid;
    pid = fork();

    if (pid == 0) {
        key_t key;
        int msqid;
        struct msgbuf msg;

        key = ftok("./buffer", 'b');

        if ((msqid = msgget(key, 0666 | IPC_CREAT)) == -1) {
            perror("msgget");
            exit(1);
        }

        msg.mtype = 1;
        strcpy(msg.mtext, "qweasdzc");

        if ((msgsnd(msqid, &msg, strlen(msg.mtext), 0)) == -1) {
            perror("msgsnd");
            exit(1);
        }

        exit(0);

    } else if (pid > 0) {
        int child_status;
        waitpid(pid, &child_status, 0);

        if (WEXITSTATUS(child_status) == 1)
            exit(1);

        key_t key;
        int msqid;
        struct msgbuf msg;

        key = ftok("./buffer", 'b');

        if ((msqid = msgget(key, 0)) == -1) {
            perror("msgget");
            exit(1);
        }

        msg.mtype = 1;

        if ((msgrcv(msqid, &msg, sizeof(msg.mtext), 0, 0)) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("%s\n", msg.mtext);

        msgctl(msqid, IPC_RMID, NULL);

    } else
        printf("Error\n");

    return 0;
}