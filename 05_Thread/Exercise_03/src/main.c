#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_ITERATIONS  10U
#define MIN_VALUE       1U
#define MAX_VALUE       10U
#define DATA_READY      1U
#define DATA_NOT_READY  0U
#define E_OK            0U
#define E_NOT_OK        1U

unsigned char data = 0;
volatile unsigned char data_flag = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void *producer(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        
        data = (rand() % MAX_VALUE) + MIN_VALUE;
        data_flag = DATA_READY;
        printf("Producer: %d\n", data);

        // Notify to consumer about new data and for consumer to receive it
        pthread_cond_signal(&cond);
        while (DATA_READY == data_flag) {
            pthread_cond_wait(&cond, &mutex);
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}
  
static void *consumer(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);

        // Wait for new data from producer
        while (DATA_NOT_READY == data_flag) { 
            pthread_cond_wait(&cond, &mutex);
        }
    
        // Get data and send signal back to producer to notify receive successful
        printf("\tConsumer: %d\n", data);
        data_flag =  DATA_NOT_READY;
        pthread_cond_signal(&cond);
    
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}


int main(int argc, char const *argv[])
{
    pthread_t producer_tid, consumer_tid;

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("pthread_mutex_init failed");
        return E_NOT_OK;
    }    

    if (pthread_create(&producer_tid, NULL, producer, NULL) != 0) {
        perror("pthread_create failed");
        return E_NOT_OK;
    }
    if (pthread_create(&consumer_tid, NULL, consumer, NULL) != 0) {
        perror("pthread_create failed");
        return E_NOT_OK;
    }
    
    // Wait for both threads are executed
    if (pthread_join(producer_tid, NULL) != 0) {
        perror("pthread_join failed");
        return E_NOT_OK;
    }
    if (pthread_join(consumer_tid, NULL) != 0) {
        perror("pthread_join failed");
        return E_NOT_OK;
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return E_OK;
}
