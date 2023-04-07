#include <stdio.h>
#include <pthread.h>
#include <signal.h>

#include "threads.h"
#include "buffer.h"
#include "cpu_data.h"

#define THREAD_NUM 5

// buffers
buffer raw_data;
buffer calculated_usage;

void sigterm_handler(int);

int main() {

  // handle exit
  struct sigaction sa;
  sa.sa_handler = sigterm_handler;

  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
  
  // initialize buffers
  init_buffer(&raw_data, sizeof(cpu_data) * (CORE_NUM + 1));
  init_buffer(&calculated_usage, sizeof(double) * (CORE_NUM + 1));
  
  // helper array to pass both to the analyzer
  void * bufs[] = { &raw_data, &calculated_usage };

  // thread pool
  pthread_t thread_pool[THREAD_NUM];

  // initialize threads
  pthread_create(&thread_pool[0], NULL, &reader, &raw_data);
  pthread_create(&thread_pool[1], NULL, &analyzer, bufs);
  pthread_create(&thread_pool[2], NULL, &printer, &calculated_usage);

  for(int i = 0; i < THREAD_NUM; i++)
    pthread_join(thread_pool[i], NULL);
}

void sigterm_handler(int num) {
  write(STDOUT_FILENO, "\nDestroying buffers...\n", 23);

  // make sure to destroy all buffers
  destroy_buffer(&raw_data);
  destroy_buffer(&calculated_usage);

  write(STDOUT_FILENO, "Done. Exiting.\n", 15);

  exit(0);
}
