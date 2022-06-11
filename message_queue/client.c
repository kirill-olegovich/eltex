#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

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
    initscr();
    curs_set(0);

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

    char temp_name[20];
    int length;
    WINDOW *login = newwin(3, 40, 1, 1);

    // refresh();
    box(login, 0, 0);
    mvwprintw(login, 1, 1, "Enter your name: ");
    wrefresh(login);
    wmove(login, 17, 1);
    wgetstr(login, temp_name);

    strcpy(my_message.message_text.name, temp_name);

    wclear(login);
    wrefresh(login);
    delwin(login);
    refresh();

    WINDOW *chat_border = newwin(20, 60, 0, 0);
    WINDOW *chat = newwin(18, 58, 1, 1);
    WINDOW *message = newwin(3, 84, 20, 0);
    WINDOW *user_list_border = newwin(20, 23, 0, 60);
    WINDOW *user_list = newwin(18, 21, 1, 61);

    box(chat_border, 0, 0);
    box(message, 0, 0);
    box(user_list_border, 0, 0);

    wmove(message, 1, 1);

    wrefresh(chat_border);
    wrefresh(chat);
    wrefresh(message);
    wrefresh(user_list_border);
    wrefresh(user_list);

    refresh();

    pid = fork();

    if (pid == 0) {
        while (1) {
            // read response from server
            if (msgrcv(myqid, &return_message, sizeof (struct message_text), 0, 0) == -1) {
                perror("msgrcv");
                exit(1);
            }

            wclear(chat);
            mvwprintw(chat, 0, 0, "%s", return_message.message_text.chat);
            wrefresh(chat);

            int j = 0;

            wclear(user_list);

            for (int i=0; i<10; i++) {
                if (return_message.message_text.user_list[i].qid != -1) {
                    mvwprintw(user_list, j, 0, "%s", return_message.message_text.user_list[i].name);
                    j++;
                }
            }

            wrefresh(user_list);

            refresh();
        }
    }

    else if (pid > 0) {
        do {
            // send message to server
            if (msgsnd(server_qid, &my_message, sizeof (struct message_text), 0) == -1) {
                perror("msgsnd");
                exit(1);
            }

            if (strncmp(my_message.message_text.buf, "exit", 4) == 0) {
                printf("Exit\n");
                break;
            }

            wgetstr(message, my_message.message_text.buf);

            wclear(message);
            box(message, 0, 0);
            wmove(message, 1, 1);
            wrefresh(message);
            refresh();
        } while (1);

        kill(pid, SIGKILL);

        // remove message queue
        if (msgctl(myqid, IPC_RMID, NULL) == -1) {
            perror ("msgctl");
            exit (1);
        }
    }

    delwin(chat);
    delwin(message);
    delwin(user_list);

    endwin();

    exit(0);
}