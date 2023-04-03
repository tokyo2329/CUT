#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "buffer.h"

void init_buffer(Buffer * b) {
  pthread_mutex_init(&b->mtx, NULL);
}

void destroy_buffer(Buffer * b) {
  pthread_mutex_destroy(&b->mtx);
}

void write_to_buffer(Buffer * b, char * d) {
  pthread_mutex_lock(&b->mtx);

  strcpy(b->data, d);

  pthread_mutex_unlock(&b->mtx);
}

char * read_from_buffer(Buffer * b) {
  pthread_mutex_lock(&b->mtx);

  char * value = malloc(BUFFER_SIZE);
  if(value == NULL) {
    printf("Failed to allocate memory!\n");
    exit(1);
  }

  strcpy(value, b->data);

  pthread_mutex_unlock(&b->mtx);

  return value;
}