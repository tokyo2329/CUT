#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

typedef struct node {
  struct node * next;
  void * values[]; // FAM (Flexible Array Members)
} node;

typedef struct {
  node * head;
  node * tail;
  size_t element_size;

  pthread_mutex_t mtx;
  sem_t sp;
} buffer;


// initializes a buffer with default values
void init_buffer(buffer * b, size_t element_size);

// destroys a buffer by freeing the remaining memory
void destroy_buffer(buffer * b);

// adds to a buffer
void enqueue(buffer * b, void * data);

// gets from a buffer
void * dequeue(buffer * b);

