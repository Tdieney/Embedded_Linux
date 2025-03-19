#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define ARRAY_SIZE      1000000U
#define NUM_OF_THREAD   4U
#define PARTIAL_SIZE    (ARRAY_SIZE / NUM_OF_THREAD)
#define MIN_VALUE       1U
#define MAX_VALUE       10U
#define E_OK            0U
#define E_NOT_OK        1U

long long global_sum = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Union to store thread data safely
typedef union {
    struct {
        unsigned int thread_id;
        unsigned char local_array[PARTIAL_SIZE];  // Copy of part of the array
    } ThreadInfo_st;
} ThreadInfo_u;

void print_array(const unsigned char *arr, unsigned long size) {
    printf("Array: ");
    for (unsigned long i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// Thread function
static void* partial_sum(void* arg) {
    ThreadInfo_u *thread_info = (ThreadInfo_u *)arg;
    long long local_sum = 0;

    for (unsigned int i = 0; i < PARTIAL_SIZE; i++) {
        local_sum += thread_info->ThreadInfo_st.local_array[i];
    }

    // Critical section: Update global sum
    if (pthread_mutex_lock(&mutex) != 0) {
        perror("pthread_mutex_lock failed");
        return NULL;
    }
    global_sum += local_sum;
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

int main() {
    pthread_t thread_ids[NUM_OF_THREAD];
    unsigned char array[ARRAY_SIZE];
    ThreadInfo_u thread_info[NUM_OF_THREAD];
    unsigned int i = 0;

    srand(time(NULL)); // Ensure different results on each run

    // Initialize an array with random values
    for (i = 0; i < ARRAY_SIZE; i++) {
        array[i] = (rand() % MAX_VALUE) + MIN_VALUE;
    }

    // Create threads with a private copy of array data
    for (i = 0; i < NUM_OF_THREAD; i++) {
        thread_info[i].ThreadInfo_st.thread_id = i;
        memcpy(thread_info[i].ThreadInfo_st.local_array, &array[i * PARTIAL_SIZE], PARTIAL_SIZE);

        if (pthread_create(&thread_ids[i], NULL, partial_sum, &thread_info[i]) != 0) {
            perror("pthread_create failed");
            return E_NOT_OK;
        }
    }
    
    // Wait for all threads to finish
    for (i = 0; i < NUM_OF_THREAD; i++) {
        if (pthread_join(thread_ids[i], NULL) != 0) {
            perror("pthread_join failed");
            return E_NOT_OK;
        }
    }

    // print_array(array, ARRAY_SIZE);
    printf("Global sum: %lld\n", global_sum);
    
    pthread_mutex_destroy(&mutex);
    return E_OK;
}
