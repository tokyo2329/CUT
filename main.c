#include <stdio.h>
#include <pthread.h>

#include "threads.h"
#include "buffer.h"
#include "cpu_data.h"


int main() {

  // initialize buffers
  buffer raw_data;
  buffer calculated_usage;
  
  // helper array to pass both to the analyzer
  void * bufs[] = { &raw_data, &calculated_usage };
  
  init_buffer(&raw_data, sizeof(cpu_data) * (CORE_NUM + 1));
  init_buffer(&calculated_usage, sizeof(double) * (CORE_NUM + 1));

  // initialize threads
  pthread_t thread_pool[3];
  pthread_create(&thread_pool[0], NULL, &reader, &raw_data);
  pthread_create(&thread_pool[1], NULL, &analyzer, bufs);
  pthread_create(&thread_pool[2], NULL, &printer, &calculated_usage);

  // join all threads
  pthread_join(thread_pool[0], NULL);
  pthread_join(thread_pool[1], NULL);
  pthread_join(thread_pool[2], NULL);

  // destory buffers
  destroy_buffer(&raw_data);
  destroy_buffer(&calculated_usage);
}
