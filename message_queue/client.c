#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <ncurses.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
#define PROJECT_ID 'M'


struct user{
    int qid;
    char name[20];
};


struct message_text {
    int qid;
    char name[20];
    char buf[200];
    char chat[1024];
    struct user user_list[10];
};


struct message {
    long message_type;
    struct message_text message_text;
};


int main (int argc, char **argv) {
    // initscr();

    pid_t pid;
    key_t server_queue_key;
    int server_qid, myqid;
    struct message my_message, return_message;

    // create my client queue for receiving messages from server
    if ((myqid = msgget (IPC_PRIVATE, 0660)) == -1) {
        perror ("msgget: myqid");
        exit (1);
    }

    if ((server_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (1);
    }

    if ((server_qid = msgget (server_queue_key, 0)) == -1) {
        perror ("msgget: server_qid");
        exit (1);
    }

    my_message.message_type = 1;
    my_message.message_text.qid = myqid;
    strcpy(my_message.message_text.buf, "init");

    // process name
    char temp[20];
    int length;

    printf("Enter your name: ");
    fgets(temp, 20, stdin);
    length = strlen(temp);
    temp[length-1] = '\0';
    strcpy(my_message.message_text.name, temp);

    pid = fork();

    if (pid == 0) {
        while (1) {
            // read response from server
            if (msgrcv(myqid, &return_message, sizeof (struct message_text), 0, 0) == -1) {
                perror("msgrcv");
                exit(1);
            }

            printf ("%s\n", return_message.message_text.chat);
        }
    }

    else if (pid > 0) {
        do {
            // send message to server
            if (msgsnd(server_qid, &my_message, sizeof (struct message_text), 0) == -1) {
                perror("msgsnd");
                exit(1);
            }

            // // read response from server
            // if (msgrcv (myqid, &return_message, sizeof (struct message_text), 0, 0) == -1) {
            //     perror ("client: msgrcv");
            //     exit (1);
            // }

            if (strncmp(my_message.message_text.buf, "exit", 4) == 0) {
                printf("Exit\n");
                break;
            }

            // printf ("%s\n", return_message.message_text.chat);
            // printf ("\n> ");

            fgets(my_message.message_text.buf, 199, stdin);

            length = strlen (my_message.message_text.buf);
            my_message.message_text.buf [length - 1] = '\0';  
        } while (1);

        kill(pid, SIGKILL);

        // remove message queue
        if (msgctl(myqid, IPC_RMID, NULL) == -1) {
            perror ("msgctl");
            exit (1);
        }
    }

    exit(0);
}