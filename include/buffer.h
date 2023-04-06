#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "cpu_data.h"

#define BUFFER_SIZE 100

typedef struct node {
  struct node * next;
  cpu_data values[]; // FAM (Flexible Array Members)
} node;

typedef struct {
  node * head;
  node * tail;

  pthread_mutex_t mtx;
  sem_t sp;
} buffer;


void init_buffer(buffer * b);

void destroy_buffer(buffer * b);

void enqueue(buffer * b, cpu_data data[]);

cpu_data * dequeue(buffer * b);

// void print(const buffer * b);
