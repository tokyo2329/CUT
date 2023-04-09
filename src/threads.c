#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "threads.h"
#include "cpu_data.h"
#include "buffer.h"


void * reader(void *) {
  FILE * file;
  char filename[] = "/proc/stat";

  while(1) {
    // send a heartbeat
    pthread_mutex_lock(&heartbeats_mtx);
    heartbeats[0]++;
    pthread_mutex_unlock(&heartbeats_mtx);

    file = fopen(filename, "r");

    // check if file is readable
    if(file == NULL) {
      printf("Can't read %s!\n", filename);
      exit(1);
    }

    // read line for total and each core
    cpu_data data[CORE_NUM + 1];
    for(int i = 0; i < CORE_NUM + 1; i++) {
      char * line = malloc(DATA_LINE_BUFFER);
      fgets(line, DATA_LINE_BUFFER, file);
      
      cpu_data * tmp = get_cpu_data(line);
      data[i] = *tmp;

      free(tmp);
      free(line);
    }

    enqueue(&raw_data, data);

    fclose(file);
    usleep(THREAD_SLEEP_USEC);
  }
}

void * analyzer(void *) {
  while(1) {
    // send a heartbeat
    pthread_mutex_lock(&heartbeats_mtx);
    heartbeats[1]++;
    pthread_mutex_unlock(&heartbeats_mtx);

    cpu_data * prev = (cpu_data *)dequeue(&raw_data);
    cpu_data * current = (cpu_data *)dequeue(&raw_data);

    double * calucalted_results = malloc(sizeof(double) * (CORE_NUM + 1));

    // calculate cpu usage for total and each core
    for(int i = 0; i < CORE_NUM + 1; i++) {
      unsigned long previous_idle = prev[i].idle + prev[i].iowait;
      unsigned long current_idle = current[i].idle + current[i].iowait;

      unsigned long previous_not_idle =
        prev[i].user + prev[i].nice + prev[i].system + prev[i].irq + prev[i].softirq + prev[i].steal;
      unsigned long current_not_idle =
        current[i].user + current[i].nice + current[i].system + current[i].irq + current[i].softirq + current[i].steal;

      unsigned long previous_total = previous_idle + previous_not_idle;
      unsigned long current_total = current_idle + current_not_idle;

      unsigned long totald = current_total - previous_total;
      unsigned long idled = current_idle - previous_idle;

      double usage = ((totald - idled) * 100.0) / (double)totald;

      calucalted_results[i] = usage;
    }

    enqueue(&calculated_usage, (void *)calucalted_results);

    free(calucalted_results);
    free(prev);
    free(current);
  }
}

void * printer(void *) {
  while(1) {
    // send a heartbeat
    pthread_mutex_lock(&heartbeats_mtx);
    heartbeats[2]++;
    pthread_mutex_unlock(&heartbeats_mtx);

    double * usage_values = (double *)dequeue(&calculated_usage);

    printf("\n\nTOTAL CPU USAGE - %.2f%%\n\n", usage_values[0]);

    for(int i = 1; i < CORE_NUM + 1; i++) {
      
      printf("CPU %d\t| ", i);

      for(int j = 0; j < usage_values[i] / 2; j++)
        printf("-");
      printf(">");

      if(usage_values[i] > 0.00)
        printf(" %.2f%%", usage_values[i]);
      printf("\n");

    }

    free(usage_values);
  }
}

void * watchdog(void * arg) {
  int thread_number = *(int *)arg;

  // set initial values
  int previous_heartbeats[thread_number];
  pthread_mutex_lock(&heartbeats_mtx);
  for(int i = 0; i < thread_number; i++)
    previous_heartbeats[i] = heartbeats[i];
  pthread_mutex_unlock(&heartbeats_mtx);

  while(1) {
    sleep(HEARTBEAT_CHECK_SEC);

    pthread_mutex_lock(&heartbeats_mtx);
    for(int i = 0; i < thread_number; i++) {
      if(heartbeats[i] == previous_heartbeats[i]) {
        printf("!!! Some threads are unresponsive, exiting !!!\n");
        exit(1);
      } else heartbeats[i]--;
    }

    for(int i = 0; i < thread_number; i++)
      previous_heartbeats[i] = heartbeats[i];

    pthread_mutex_unlock(&heartbeats_mtx);
  }
}
