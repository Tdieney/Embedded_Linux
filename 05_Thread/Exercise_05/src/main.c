#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_READERS     5U
#define NUM_WRITERS     2U
#define NUM_WRITES      5U
#define ONE             1U
#define E_OK            0U
#define E_NOT_OK        1U


unsigned int data = 0;
pthread_rwlock_t rwlock;

static void* reader(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < NUM_WRITES; i++) {
        pthread_rwlock_rdlock(&rwlock);
        printf("Reader %d: Read data = %d\n", id, data);
        pthread_rwlock_unlock(&rwlock);
        sleep(2);
    }
    return NULL;
}

static void* writer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < NUM_WRITES; i++) {
        pthread_rwlock_wrlock(&rwlock);
        data++;
        printf("Writer %d: Incremented data to %d\n", id, data);
        pthread_rwlock_unlock(&rwlock);
        sleep(2);
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_t reader_tids[NUM_READERS], writer_tids[NUM_WRITERS];
    unsigned int reader_ids[NUM_READERS], writer_ids[NUM_WRITERS];
    unsigned int i;    

    for (i = 0; i < NUM_READERS; i++) {
        reader_ids[i] = i + ONE;
        if (pthread_create(&reader_tids[i], NULL, reader, &reader_ids[i]) != 0) {
            printf("pthread_create failed");
            return E_NOT_OK;
        }
    }
    for (i = 0; i < NUM_WRITERS; i++) {
        writer_ids[i] = i + ONE;
        if (pthread_create(&writer_tids[i], NULL, writer, &writer_ids[i]) != 0) {
            printf("pthread_create failed");
            return E_NOT_OK;
        }
    }

    for (i = 0; i < NUM_READERS; i++) {
        if (pthread_join(reader_tids[i], NULL) != 0) {
            printf("pthread_join failed");
            return E_NOT_OK;
        }
    }
    for (i = 0; i < NUM_WRITERS; i++) {
        if (pthread_join(writer_tids[i], NULL) != 0) {
            printf("pthread_join failed");
            return E_NOT_OK;
        }
    }

    if (pthread_rwlock_destroy(&rwlock) != 0) {
        printf("pthread_rwlock_destroy failed");
        return E_NOT_OK;
    };

    printf("Final data value: %d\n", data);

    return E_OK;
}
