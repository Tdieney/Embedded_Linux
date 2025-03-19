#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define ARRAY_SIZE      100U
#define MIN_VALUE       1U
#define MAX_VALUE       100U
#define E_OK            0U
#define E_NOT_OK        1U

void print_array(const unsigned char *arr, size_t size) {
    printf("Array: ");
    for (size_t i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

static void *count_even(void *arg) {
    unsigned char *numbers = (unsigned char *)arg;
    unsigned char *even_counter = malloc(sizeof(unsigned char)); // Allocate memory for the result
    if (!even_counter) {
        perror("malloc failed");
        pthread_exit(NULL);
    }

    *even_counter = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (numbers[i] % 2 == 0) {
            (*even_counter)++;
        }
    }
    pthread_exit(even_counter); // Return pointer to allocated memory
}
  
static void *count_odd(void *arg) {
    unsigned char *numbers = (unsigned char *)arg;
    unsigned char *odd_counter = malloc(sizeof(unsigned char)); // Allocate memory for the result
    if (!odd_counter) {
        perror("malloc failed");
        pthread_exit(NULL);
    }

    *odd_counter = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (numbers[i] % 2 != 0) {
            (*odd_counter)++;
        }
    }
    pthread_exit(odd_counter); // Return pointer to allocated memory
}


int main(int argc, char const *argv[])
{
    srand(time(NULL)); // Seed the random number generator
    // Generate an array of random numbers (1-100)
    unsigned char numbers[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++) {
        numbers[i] = (rand() % MAX_VALUE) + 1;
    }
    
    pthread_t even_tid, odd_tid;
    unsigned char *even_counter, *odd_counter;

    if (pthread_create(&even_tid, NULL, count_even, numbers) != 0) {
        perror("pthread_create failed");
        return E_NOT_OK;
    }
    if (pthread_create(&odd_tid, NULL, count_odd, numbers) != 0) {
        perror("pthread_create failed");
        return E_NOT_OK;
    }
    
    // Wait for both threads are executed
    if (pthread_join(even_tid, (void **)&even_counter) != 0) {
        perror("pthread_join failed");
        return E_NOT_OK;
    }
    if (pthread_join(odd_tid, (void **)&odd_counter) != 0) {
        perror("pthread_join failed");
        return E_NOT_OK;
    }

    // Print results
    print_array(numbers, ARRAY_SIZE);
    printf("Even count: %d\n", *even_counter);
    printf("Odd count: %d\n", *odd_counter);

    return E_OK;
}
