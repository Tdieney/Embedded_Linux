#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_t tid_producers[10];
pthread_t tid_consumers[5];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE];

int putptr = 0;
int takeptr = 0;
int count = 0;

void *producer(void *arg) {
  int loop = BUFFER_SIZE * 2;
  while (loop-- > 0) {
    pthread_mutex_lock(&mutex);
    if (count == BUFFER_SIZE)
      pthread_cond_wait(&not_full, &mutex);
    buffer[putptr] = rand() % 100;
    count++;
    printf("Producer: %d\n", buffer[putptr]);
    putptr = (putptr + 1) % BUFFER_SIZE;
    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

void *consumer(void *arg) {
  int value;

  int loop = BUFFER_SIZE * 2;
  while (loop-- > 0) {
    pthread_mutex_lock(&mutex);
    if (count == 0)
      pthread_cond_wait(&not_empty, &mutex);
    value = buffer[takeptr];
    printf("\tConsumer: %d\n", value);
    buffer[takeptr] = -1;
    count--;
    takeptr = (takeptr + 1) % BUFFER_SIZE;
    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

int main(void) {
  for (int i = 0; i < 10; i++) {
    pthread_create(&tid_producers[i], NULL, producer, NULL);
  }
  for (int i = 0; i < 5; i++) {
    pthread_create(&tid_consumers[i], NULL, consumer, NULL);
  }

  for (int i = 0; i < 10; i++) {
    pthread_join(tid_producers[i], NULL);
  }
  for (int i = 0; i < 5; i++) {
    pthread_join(tid_consumers[i], NULL);
  }

  return EXIT_SUCCESS;
}
