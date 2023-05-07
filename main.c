#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#include "threads.h"
#include "buffer.h"
#include "cpu_data.h"
#include "log_data.h"

#define THREAD_NUM 5


// buffers
buffer raw_data, calculated_usage, logs;

// 'heartbeat' array (except watchdog)
volatile int heartbeats[THREAD_NUM - 1];
pthread_mutex_t heartbeats_mtx;


void sigterm_handler(int);


int main() {

  // handle SIGTERM
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sigterm_handler;
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);
  
  // initialize buffers
  init_buffer(&raw_data, sizeof(cpu_data) * (CORE_NUM + 1));
  init_buffer(&calculated_usage, sizeof(double) * (CORE_NUM + 1));
  init_buffer(&logs, sizeof(log_data));

  // initialize watchdog dependencies
  pthread_mutex_init(&heartbeats_mtx, NULL);
  int number_of_threads = THREAD_NUM - 1; // except watchdog // helper to pass to thread
  for(int i = 0; i < number_of_threads; i++)
    heartbeats[i] = 0; // set initial values

  // thread pool
  pthread_t thread_pool[THREAD_NUM];

  // initialize threads
  pthread_create(&thread_pool[0], NULL, &reader, NULL);
  pthread_create(&thread_pool[1], NULL, &analyzer, NULL);
  pthread_create(&thread_pool[2], NULL, &printer, NULL);
  pthread_create(&thread_pool[3], NULL, &watchdog, (void *)&number_of_threads);
  pthread_create(&thread_pool[4], NULL, &logger, NULL);

  // join threads
  for(int i = 0; i < THREAD_NUM; i++)
    pthread_join(thread_pool[i], NULL);
}

void sigterm_handler(int num) {
  write(STDOUT_FILENO, "\nDestroying buffers...\n", 23);

  // make sure to destroy all buffers
  destroy_buffer(&raw_data);
  destroy_buffer(&calculated_usage);
  destroy_buffer(&logs);

  // destory heartbeat mutex
  pthread_mutex_destroy(&heartbeats_mtx);

  write(STDOUT_FILENO, "Done. Exiting.\n", 15);
  exit(0);
}
