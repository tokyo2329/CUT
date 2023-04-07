#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "threads.h"
#include "cpu_data.h"
#include "buffer.h"


void * reader(void * arg) {
  buffer * buf = (buffer *)arg;
  
  FILE * file;
  char filename[] = "/proc/stat";

  while(1) {
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

    enqueue(buf, data);

    fclose(file);
    sleep(THREAD_SLEEP_SEC);
  }
}

void * analyzer(void * arg) {
  void ** args = (void **)arg;

  buffer * buf = (buffer *)args[0];
  buffer * out_buf = (buffer *)args[1];

  while(1) {
    cpu_data * prev = (cpu_data *)dequeue(buf);
    cpu_data * current = (cpu_data *)dequeue(buf);

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
      // printf("CPU[%d] USAGE: %.2f%%\n", i, usage);
    }

    enqueue(out_buf, (void *)calucalted_results);

    free(calucalted_results);
    free(prev);
    free(current);
  }
}

void * printer(void * arg) {
  buffer * b = (buffer *)arg;

  while(1) {
    double * usage_values = (double *)dequeue(b);

    printf("\n\nTOTAL CPU USAGE - %.2f%%\n\n", usage_values[0]);

    for(int i = 1; i < CORE_NUM + 1; i++) {
      
      printf("CPU %d\t| ", i);

      for(int j = 0; j <= usage_values[i] / 2; j++)
        printf("-");
      printf("> %.2f%%\n", usage_values[i]);
      
    }

    free(usage_values);
  }
}
