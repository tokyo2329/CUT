#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>

#include "buffer.h"

void init_buffer(buffer * b, size_t element_size) {
  
  // init mutex and semaphore
  pthread_mutex_init(&b->mtx, NULL);
  sem_init(&b->sp, 0, 0);

  // init queue
  b->head = NULL;
  b->tail = NULL;
  b->element_size = element_size;
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

void enqueue(buffer * b, void * data) {
  
  pthread_mutex_lock(&b->mtx);

  // init new node
  node * new_node = malloc(sizeof(node) + b->element_size);
  
  // fail if malloc fails to allocate memory
  if(new_node == NULL)
    exit(1);
  
  memcpy(new_node->values, data, b->element_size);
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

void * dequeue(buffer * b) {
  sem_wait(&b->sp);
  pthread_mutex_lock(&b->mtx);

  // check if queue not empty
  if(b->head == NULL)
    return NULL;
  
  // fetch the value
  void * rtn_value = malloc(b->element_size);
  
  // fail if malloc fails to allocate memory
  if(rtn_value == NULL)
    exit(1);
  
  memcpy(rtn_value, b->head->values, b->element_size);

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
