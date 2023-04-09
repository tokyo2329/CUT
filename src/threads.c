#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "threads.h"
#include "cpu_data.h"
#include "log_data.h"
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

    log_data read_log = { .thread_name = "Reader", .message = "Read /proc/stat" };
    enqueue(&logs, &read_log);

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

    log_data write_log = { .thread_name = "Reader", .message = "Wrote to buffer" };
    enqueue(&logs, &write_log);
    
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

    log_data read_log = { .thread_name = "Analyzer", .message = "Read 2 values from buffer" };
    enqueue(&logs, &read_log);

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

    log_data write_log = { .thread_name = "Analyzer", .message = "Calculated usage and added to buffer" };
    enqueue(&logs, &write_log);

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
        char * log_message;
        
        switch (i) {
          case 0:
            log_message = "Reader seems unresponsive";
            break;
          case 1:
            log_message = "Analyzer seems unresponsive";
            break;
          case 2:
            log_message = "Printer seems unresponsive";
            break;
          case 3:
            log_message = "Watchdog seems unresponsive";
            break;
          case 4:
            log_message = "Logger seems unresponsive";
            break;
          default:
            log_message = "Something went wrong";
            break;
        }

        log_data unresponsive_log = { .thread_name = "Watchdog", .message = log_message };
        enqueue(&logs, &unresponsive_log);

      } else heartbeats[i]--;
    }

    for(int i = 0; i < thread_number; i++)
      previous_heartbeats[i] = heartbeats[i];

    pthread_mutex_unlock(&heartbeats_mtx);
  }
}

void * logger(void *) {
  char filename[] = "logs.txt";
  FILE * f = fopen(filename, "w");
  
  while(1) {
    // send a heartbeat
    pthread_mutex_lock(&heartbeats_mtx);
    heartbeats[3]++;
    pthread_mutex_unlock(&heartbeats_mtx);

    // check if file is writable
    if(f == NULL) {
      printf("Can't write to %s!\n", filename);
      exit(1);
    }

    log_data * log = (log_data *)dequeue(&logs);
    fprintf(f, "[CUT] Thread: %s -> message: %s\n", log->thread_name, log->message);
    
    free(log);
  }

  fclose(f);
}
