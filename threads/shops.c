#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int shops[5];
int clients[3];
int threads[3];
pthread_mutex_t mutexes[5] = {PTHREAD_MUTEX_INITIALIZER};

void *func(void *arg) {
    int id = *((int *)arg);

    for (int i = 0; i < 5; i++) {
        if (clients[id] < 1)
            break;

        if (pthread_mutex_trylock(&mutexes[i]) == 0) {
            int or_client = clients[id];
            int or_shop = shops[i];

            if (shops[i] == 0)
                continue;

            if (clients[id] >= shops[i]) {
                clients[id] -= shops[i];
                shops[i] = 0;
            } else {
                shops[i] -= clients[id];
                clients[id] = 0;
            }

            pthread_mutex_unlock(&mutexes[i]);
            printf("Client [%d] spent %d$ (was %d$, now %d$) in shop [%d] (was "
                   "%d$, now %d$)\n",
                   id + 1, or_client - clients[id], or_client, clients[id],
                   i + 1, or_shop, shops[i]);
            sleep(2);
        } else {
            if (i == 2)
                i = 0;
        }
    }

    return NULL;
}

void *func2(void *arg) {
    for (int i = 0; i < 5; i++) {
        if (clients[0] == 0 && clients[1] == 0 && clients[2] == 0)
            break;

        if (pthread_mutex_trylock(&mutexes[i]) == 0) {
            int or_shop = shops[i];

            shops[i] += 500;
            pthread_mutex_unlock(&mutexes[i]);
            printf("Loader have brought 500$ to shop [%d] (was %d$, now %d$)\n",
                   i + 1, or_shop, shops[i]);
            sleep(1);
        } else {
            if (i == 4)
                i = 0;
        }
    }

    return NULL;
}

int main(void) {
    srand(time(NULL));

    pthread_t thread[4];

    for (int i = 0; i < 3; i++)
        pthread_mutex_init(&mutexes[i], NULL);

    for (int i = 0; i < 5; i++)
        shops[i] = (rand() % 101) + 1000;

    for (int i = 0; i < 3; i++)
        clients[i] = (rand() % 101) + 2000;

    for (int i = 0; i < 3; i++) {
        threads[i] = i;
        pthread_create(&thread[i], NULL, func, &threads[i]);
    }

    pthread_create(&thread[3], NULL, func2, NULL);

    for (int i = 0; i < 3; i++)
        pthread_join(thread[i], NULL);

    pthread_join(thread[3], NULL);

    for (int i = 0; i < 3; i++)
        pthread_mutex_destroy(&mutexes[i]);

    return 0;
}
