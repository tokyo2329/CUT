#pragma once

#include "buffer.h"

#define THREAD_SLEEP_SEC 1
#define DATA_LINE_BUFFER 1024

typedef struct {
  buffer * b1;
  buffer * b2;
} buffers;

/*
* Reader thread
* It's job is to read, convert and send data from /proc/stat to the buffer
*/
void * reader(void * arg);

/*
* Analyzer thread
* It's job is to analyze and calculate CPU usage from the raw data buffer
* then send it to a different buffer
*/
void * analyzer(void * arg);

/*
* Printer thread
* It's job is to print the calculated usage to the console
*/
void * printer(void * arg);
