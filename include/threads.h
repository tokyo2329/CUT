#pragma once

#include "buffer.h"

#define THREAD_SLEEP_USEC 500000
#define HEARTBEAT_CHECK_SEC 3
#define DATA_LINE_BUFFER 1024

extern buffer raw_data, calculated_usage, logs;

extern volatile int heartbeats[];
extern pthread_mutex_t heartbeats_mtx;

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

/*
* Watchdog thread
* It's job is to listen for heartbeats from other threads and exit
* if considers any unresponsive
*/
void * watchdog(void * arg);

/*
* Logger thread
* It's job is to collect log data from other threads and output them to a file
*/
void * logger(void * arg);
