#pragma once

#include <pthread.h>

#define BUFFER_SIZE 100

typedef struct {
  char data[BUFFER_SIZE];
  pthread_mutex_t mtx;
} Buffer;

void init_buffer(Buffer * b);

void destroy_buffer(Buffer * b);

void write_to_buffer(Buffer * b, char * d);

char * read_from_buffer(Buffer * b);