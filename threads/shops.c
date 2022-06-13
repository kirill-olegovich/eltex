#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

long t = 0;
int shops[5], customers[3];
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int check_customers(void) {
    for (int i = 0; i < 3; ++i)
        if (customers[i] > 0)
            return 0;
    return 1;
}

void *pthread_func(void *args) {

    pthread_mutex_lock(&lock);

    if (!check_customers() && t == 0) {
        t = 1;

        printf("Start\n");

        pthread_cond_wait(&cond, &lock);
    }

    pthread_mutex_unlock(&lock);
    printf("End\n");

    return NULL;
}

int main(void) {
    srand(time(NULL));

    pthread_t tid1, tid2;

    for (int i = 0; i < 5; i++)
        shops[i] = (rand() % 101) + 950;
    for (int i = 0; i < 3; i++)
        customers[i] = (rand() % 1001) + 4500;

    pthread_create(&tid1, NULL, pthread_func, NULL);
    // sleep(1);
    // pthread_create(&tid2, NULL, pthread_func, NULL);

    pthread_join(tid2, NULL);

    return 0;
}