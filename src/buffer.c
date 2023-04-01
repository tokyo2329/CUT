#include <pthread.h>
#include "buffer.h"

void init_buffer(Buffer * b) {
  pthread_mutex_init(&b->mtx, NULL);
}

void destroy_buffer(Buffer * b) {
  pthread_mutex_destroy(&b->mtx);
}

void write_to_buffer(Buffer * b, char * d) {
  pthread_mutex_lock(&b->mtx);
  *(b->data) = *d;
  pthread_mutex_unlock(&b->mtx);
}

char * read_from_buffer(Buffer * b) {
  pthread_mutex_lock(&b->mtx);
  char * value = b->data;
  pthread_mutex_unlock(&b->mtx);

  return value;
}