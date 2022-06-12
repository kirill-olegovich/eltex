#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

long a = 0;

void *pthread_func(void *args) {
	// int *index;
	// index = (int *) args;
	// printf("index = %d\n", *index);
	for (int i=0; i<100000; i++) a++;

	return NULL;
}

int main(int argc, char *argv[]) {
	int N, opt;
	int *s;

	N = 100;

	while ((opt = getopt(argc, argv, "N:")) != -1) {
		switch (opt) {
		    case 'N':
		        N = strtoul(optarg, NULL, 0);
		        break;

		    default:
		        fprintf(stderr, "Usage: %s [-N number of threads] arg...\n", argv[0]);
		        exit(EXIT_FAILURE);
		}
	}

	int index[N];
	pthread_t thread[N];

	for (int i=0; i<N; i++) {
		index[i] = i+1;
		pthread_create(&thread[i], NULL, pthread_func, &index[i]);
	}

	for (int i=0; i<N; i++) {
		pthread_join(thread[i], (void **) &s);
	}

	printf("%ld\n", a);

	return 0;
}