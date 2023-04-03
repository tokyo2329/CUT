#pragma once

#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 100

typedef struct {
  char data[BUFFER_SIZE];
  pthread_mutex_t mtx;
  sem_t sp;
} buffer;

typedef struct {
  unsigned long
  user, nice, system, idle, iowait,
  irq, softirq, steal, guest, guest_nice;
} cpu_data;

void init_buffer(buffer * b);

void destroy_buffer(buffer * b);

void write_to_buffer(buffer * b, char * d);

char * read_from_buffer(buffer * b);