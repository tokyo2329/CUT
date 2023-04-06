#pragma once

#define CORE_NUM sysconf(_SC_NPROCESSORS_ONLN)

typedef struct {
  unsigned long
  user, nice, system, idle, iowait,
  irq, softirq, steal, guest, guest_nice;
} cpu_data;

// get cpu_data structure from raw data string
cpu_data * get_cpu_data(char * str);
