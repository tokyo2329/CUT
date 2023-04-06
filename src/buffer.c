#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>

#include "buffer.h"

void init_buffer(buffer * b) {
  
  // init mutex and semaphore
  pthread_mutex_init(&b->mtx, NULL);
  sem_init(&b->sp, 0, 0);

  // init queue
  b->head = NULL;
  b->tail = NULL;
}

void destroy_buffer(buffer * b) {
  
  // make sure the queue remainings are freed
  node * iter = b->head;

  while(iter != NULL) {
    node * tmp = iter;
    iter = iter->next;
    free(tmp);
  }

  // reset queue
  b->head = NULL;
  b->tail = NULL;
  
  pthread_mutex_destroy(&b->mtx);
  sem_destroy(&b->sp);
}

void enqueue(buffer * b, cpu_data data[]) {
  
  pthread_mutex_lock(&b->mtx);

  // init new node
  node * new_node = malloc(sizeof(node) + (sizeof(cpu_data) * (CORE_NUM + 1)));
  
  // fail if malloc fails to allocate memory
  if(new_node == NULL)
    exit(1);
  
  memcpy(new_node->values, data, sizeof(cpu_data) * (CORE_NUM + 1));
  new_node->next = NULL;

  // add node to queue
  if(b->tail != NULL)
    b->tail->next = new_node;

  b->tail = new_node;

  // if queue was empty - assign head
  if(b->head == NULL)
    b->head = new_node;

  pthread_mutex_unlock(&b->mtx);
  sem_post(&b->sp);
}

cpu_data * dequeue(buffer * b) {
  sem_wait(&b->sp);
  pthread_mutex_lock(&b->mtx);

  // check if queue not empty
  if(b->head == NULL)
    return NULL;
  
  // fetch the value
  cpu_data * rtn_value = malloc(sizeof(cpu_data) * (CORE_NUM + 1));
  
  // fail if malloc fails to allocate memory
  if(rtn_value == NULL)
    exit(1);
  
  memcpy(rtn_value, b->head->values, sizeof(cpu_data) * (CORE_NUM + 1));

  // move the queue
  node * tmp = b->head;
  b->head = b->head->next;
  free(tmp);

  // if queue empty assign tail
  if(b->head == NULL)
    b->tail = NULL;
  
  pthread_mutex_unlock(&b->mtx);

  return rtn_value;
}

// void print(const buffer * b) {
//   buffer_node * iter = b->head;
  
//   while(iter != NULL) {
//     printf("%s, ", iter->value);
//     iter = iter->next;
//   }
//   printf("\n");
// }
