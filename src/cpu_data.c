#include <string.h>

#include "cpu_data.h"

cpu_data * get_cpu_data(char * str) {

  char * raw_data = strdup(str);

  cpu_data * data = malloc(sizeof(cpu_data));

  char * cursor = raw_data + 5; //offset "cpuX "
  memcpy(data, &(cpu_data){
    .user = strtoul(cursor, &cursor, 10),
    .nice = strtoul(cursor, &cursor, 10),
    .system = strtoul(cursor, &cursor, 10),
    .idle = strtoul(cursor, &cursor, 10),
    .iowait = strtoul(cursor, &cursor, 10),
    .irq = strtoul(cursor, &cursor, 10),
    .softirq = strtoul(cursor, &cursor, 10),
    .steal = strtoul(cursor, &cursor, 10),
    .guest = strtoul(cursor, &cursor, 10),
    .guest_nice = strtoul(cursor, &cursor, 10)
  }, sizeof(cpu_data));

  free(raw_data);
  return data;
}
