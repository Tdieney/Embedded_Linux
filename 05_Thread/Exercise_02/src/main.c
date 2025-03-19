#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define ONE_MILLION     1000000U
#define NUM_OF_THREAD   3U
#define E_OK            0U
#define E_NOT_OK        1U

unsigned long counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void *thread_function(void *args) 
{    
    for (unsigned int i = 0; i < ONE_MILLION; i++) {
        pthread_mutex_lock(&mutex);
        counter++; // Critical section
        pthread_mutex_unlock(&mutex);
    }
}


int main(int argc, char const *argv[])
{
    pthread_t thread_id[NUM_OF_THREAD];
    unsigned int i = 0;

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("pthread_mutex_init failed");
        return E_NOT_OK;
    }

    for (i = 0; i < NUM_OF_THREAD; i++) {
        if (pthread_create(&thread_id[i], NULL, thread_function, NULL) != 0) {
            perror("pthread_create failed");
            return E_NOT_OK;
        }
    }
    
    // Wait for both threads are executed
    for (i = 0; i < NUM_OF_THREAD; i++) {
        if (pthread_join(thread_id[i], NULL) != 0) {
            perror("pthread_join failed");
            return E_NOT_OK;
        }
    }

    printf("Counter: %lu\n", counter);
    pthread_mutex_destroy(&mutex);

    return E_OK;
}
