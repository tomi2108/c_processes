#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define BUFF_SIZE 10

int buffer[BUFF_SIZE];
int count = 0;
pthread_mutex_t buffer_mutex;
sem_t *sem_empty;
sem_t *sem_full;

void *producer(void *arg) {
  int id = *(int *)arg;
  while (1) {
    int x = rand() % 99;
    sem_wait(sem_empty);
    pthread_mutex_lock(&buffer_mutex);
    buffer[count] = x;
    count++;
    pthread_mutex_unlock(&buffer_mutex);
    sem_post(sem_full);
    printf("(%02d): Posted %d\n", id, x);
    sleep(1);
  }
  free(arg);
  return NULL;
}

void *consumer(void *arg) {
  int id = *(int *)arg;
  while (1) {
    int y;
    sem_wait(sem_full);
    pthread_mutex_lock(&buffer_mutex);
    count--;
    y = buffer[count];
    pthread_mutex_unlock(&buffer_mutex);
    sem_post(sem_empty);
    printf("(%02d): Consumed %d \n", id, y);
    sleep(1);
  }
  free(arg);
  return NULL;
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
  int producers;
  int consumers;
  printf("Input ammount of producers:\n");
  scanf("%d", &producers);
  printf("Input ammount of consumers:\n");
  scanf("%d", &consumers);
  int threads = producers + consumers;

  pthread_t th[threads];
  pthread_mutex_init(&buffer_mutex, NULL);
  sem_unlink("sem_empty");
  sem_unlink("sem_full");

  sem_empty = sem_open("sem_empty", O_CREAT, 0644, BUFF_SIZE);
  sem_full = sem_open("sem_full", O_CREAT, 0644, 0);

  int i = 0;
  while (i < threads) {
    if (i < producers) {
      int *producer_id = malloc(sizeof(int));
      *producer_id = i + 1;
      pthread_create(&th[i], NULL, &producer, producer_id);
    } else {
      int *consumer_id = malloc(sizeof(int));
      *consumer_id = i + 1;
      pthread_create(&th[i], NULL, &consumer, consumer_id);
    }
    i++;
  }

  for (int j = 0; j < threads; j++) {
    pthread_join(th[j], NULL);
  }
  sem_close(sem_empty);
  sem_close(sem_full);
  pthread_mutex_destroy(&buffer_mutex);

  return 0;
}