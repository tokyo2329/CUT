#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include "buffer.h"

void init_buffer(buffer * b) {
  pthread_mutex_init(&b->mtx, NULL);
  sem_init(&b->sp, 0, 0);
}

void destroy_buffer(buffer * b) {
  pthread_mutex_destroy(&b->mtx);
  sem_destroy(&b->sp);
}

void write_to_buffer(buffer * b, char * d) {
  pthread_mutex_lock(&b->mtx);
  strcpy(b->data, d);
  pthread_mutex_unlock(&b->mtx);
  
  sem_post(&b->sp);
}

char * read_from_buffer(buffer * b) {
  sem_wait(&b->sp);

  pthread_mutex_lock(&b->mtx);
  char * from_buffer = strdup(b->data);
  pthread_mutex_unlock(&b->mtx);

  return from_buffer;
}