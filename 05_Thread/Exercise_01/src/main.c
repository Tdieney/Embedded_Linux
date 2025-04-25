#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_OF_THREAD   2U
#define E_OK            0U
#define E_NOT_OK        1U

pthread_t thread_id[NUM_OF_THREAD];

static void *thread_function(void *args) 
{
    /* 
        /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h 
        line 27: typedef unsigned long int pthread_t;
    */
    pthread_t thread_id = pthread_self();
    printf("Thread ID: %lu. Hello from thread.\n", thread_id);
}


int main(int argc, char const *argv[])
{
    unsigned int i = 0;

    for (i = 0; i < NUM_OF_THREAD; i++) {
        if (pthread_create(&thread_id[i], NULL, thread_function, NULL) != 0) {
            printf("pthread_create failed");
            return E_NOT_OK;
        }
    }
    
    // Wait for both threads are executed
    for (i = 0; i < NUM_OF_THREAD; i++) {
        if (pthread_join(thread_id[i], NULL) != 0) {
            printf("pthread_join failed");
            return E_NOT_OK;
        }
    }

    return E_OK;
}
