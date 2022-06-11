#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
#define PROJECT_ID 'M'
#define QUEUE_PERMISSIONS 0660


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


void update_users(struct user user_list[10], char name[20], int qid, int action) {
	if (action == 0) {
		for (int i=0; i<10; i++) {
			if (user_list[i].qid == -1) {
				user_list[i].qid = qid;
				strcpy(user_list[i].name, name);
				break;
			}
		}
	}

	else {
		for (int i=0; i<10; i++) {
			if (user_list[i].qid == qid) {
				user_list[i].qid = -1;
				memset(user_list[i].name, 0, 20);
				break;
			}
		}
	}
}


int main (int argc, char **argv) {
    key_t msg_queue_key;
    int qid;
    pid_t pid;
    struct message my_message;
    struct user user_list[10];
    char chat[1024];

    memset(chat, 0, 1024);

    if ((msg_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (1);
    }

    if ((qid = msgget (msg_queue_key, IPC_CREAT | QUEUE_PERMISSIONS)) == -1) {
        perror ("msgget");
        exit (1);
    }

    //initialize user_list
    for (int i=0; i<10; i++) {
    	user_list[i].qid = -1;
		memset(user_list[i].name, 0, 20);
    }

    pid = fork();

    if (pid == 0) {
	    while (1) {
	        // read an incoming message
	        if (msgrcv (qid, &my_message, sizeof (struct message_text), 0, 0) == -1) {
	            perror ("msgrcv");
	            exit (1);
	        }

	        char buffer[1024];

	        if (strncmp(my_message.message_text.buf, "exit", 4) == 0) {
		    	update_users(user_list, my_message.message_text.name, my_message.message_text.qid, 1);
	        	sprintf(buffer, "<%s> left the chat\n", my_message.message_text.name);
	        }
	        
	        else if (strncmp(my_message.message_text.buf, "init", 4) == 0) {
		    	update_users(user_list, my_message.message_text.name, my_message.message_text.qid, 0);
	        	sprintf(buffer, "<%s> has joined\n", my_message.message_text.name);
	        }
			
			else sprintf(buffer, "%s: %s\n", my_message.message_text.name, my_message.message_text.buf);

			int client_qid;

	        strcat(chat, buffer);
	        strcpy(my_message.message_text.chat, chat);
	        memcpy(my_message.message_text.user_list, user_list, sizeof(struct user) * 10);
			
	        for (int i=0; i<10; i++) {
	        	if (user_list[i].qid == -1) continue;

	        	client_qid = user_list[i].qid;
	        
		        // send reply message to client
		        if (msgsnd (client_qid, &my_message, sizeof(struct message_text), 0) == -1) {  
		            perror ("msgsnd");
		            exit (1);
		        }

		        printf("sending to <%s>\n", user_list[i].name);
	        }

	        printf("\n");
	    }
    }

    else if (pid > 0) {
    	char input[10];

    	do {
    		fgets(input, 10, stdin);
    	} while (strncmp(input, "stop", 4) != 0);

    	kill(pid, SIGKILL);
    }

    exit(0);
}