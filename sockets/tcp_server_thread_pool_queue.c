#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

pthread_t thread_pool[5];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct node {
    struct node *next;
    int client_socket;
};

struct node *head = NULL;
struct node *tail = NULL;

void communication(int connectedfd);
void *thread_func(void *arg);
void push(int client_socket);
int pop();

int main(void) {
    int socketfd, length, connectedfd;
    struct sockaddr_in serveraddr, clientaddr;
    int *s;

    for (int i = 0; i < 5; i++)
        pthread_create(&thread_pool[i], NULL, thread_func, NULL);

    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8888);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socketfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) ==
        -1) {
        perror("bind");
        exit(1);
    }

    listen(socketfd, 5);

    while (1) {
        length = sizeof(clientaddr);
        connectedfd = accept(socketfd, (struct sockaddr *)&clientaddr, &length);
        printf("User connected\n");

        push(connectedfd);
    }

    close(socketfd);

    return 0;
}

void communication(int connectedfd) {
    char buffer[1024];

    bzero(buffer, 1024);

    read(connectedfd, buffer, 1024);

    printf("From client (%d): %s\n", connectedfd, buffer);

    write(connectedfd, "OK", 2);

    sleep(1);
}

void *thread_func(void *arg) {
    while (1) {
        pthread_mutex_lock(&lock);

        int pclient = pop();

        pthread_mutex_unlock(&lock);

        if (pclient != -1) {
            communication(pclient);
        }
    }
}

void push(int client_socket) {
    struct node *newnode = malloc(sizeof(struct node));

    newnode->client_socket = client_socket;
    newnode->next = NULL;

    if (head == NULL) {
        head = newnode;
        tail = newnode;
    }

    else {
        head->next = newnode;
        head = newnode;
    }
}

int pop() {
    if (tail == NULL)
        return -1;

    struct node *p;
    int result = tail->client_socket;

    p = tail;
    tail = tail->next;

    free(p);

    if (tail == NULL)
        head = NULL;

    return result;
}