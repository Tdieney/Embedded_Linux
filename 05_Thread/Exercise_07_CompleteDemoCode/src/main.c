#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE         5U
#define PRODUCER_COUNT      10U
#define CONSUMER_COUNT      10U
#define ITEMS_PER_THREAD    10U  // Ensures balanced work

pthread_t tid_producers[PRODUCER_COUNT];
pthread_t tid_consumers[CONSUMER_COUNT];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

int buffer[BUFFER_SIZE];

int putptr = 0;
int takeptr = 0;
int count = 0;

void *producer(void *arg) {
    int loop = ITEMS_PER_THREAD;
    while (loop-- > 0) {
        int item = rand() % 100;

        pthread_mutex_lock(&mutex);
        while (count == BUFFER_SIZE)  // Use while to prevent spurious wakeups
            pthread_cond_wait(&not_full, &mutex);

        buffer[putptr] = item;
        putptr = (putptr + 1) % BUFFER_SIZE;
        count++;

        printf("Producer %ld: %d\n", (long)arg, buffer[putptr]);
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumer(void *arg) {
    int value;
    int loop = ITEMS_PER_THREAD * (PRODUCER_COUNT / CONSUMER_COUNT);  // Ensure equal work

    while (loop-- > 0) {
        pthread_mutex_lock(&mutex);
        while (count == 0)  // Use while instead of if
            pthread_cond_wait(&not_empty, &mutex);

        value = buffer[takeptr];
        takeptr = (takeptr + 1) % BUFFER_SIZE;
        count--;

        printf("\tConsumer %ld: %d\n", (long)arg, value);
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void) {
    srand(time(NULL));  // Seed random number generator

    for (long i = 0; i < PRODUCER_COUNT; i++) {
        pthread_create(&tid_producers[i], NULL, producer, (void *)i);
    }
    for (long i = 0; i < CONSUMER_COUNT; i++) {
        pthread_create(&tid_consumers[i], NULL, consumer, (void *)i);
    }

    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_join(tid_producers[i], NULL);
    }
    for (int i = 0; i < CONSUMER_COUNT; i++) {
        pthread_join(tid_consumers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);

    return EXIT_SUCCESS;
}
