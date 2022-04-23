#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "queue.h"
#include "base_tx.h"
#include "base_block.h"
#include <unistd.h>

Queue *queue_init(){
  Queue *new_q = malloc(sizeof(Queue));
  new_q->head = NULL;
  new_q->tail = NULL;
  //INitialize sepmaphores and lock
  new_q->semaphore = 0;//0
  if (pthread_mutex_init(&new_q->lock, NULL) != 0) {
      printf("\n mutex init has failed\n");
      return NULL;
  }
  //pthread_mutex_unlock(&new_q->lock);
  return new_q;
}

void queue_destroy(Queue *existing_q){
  pthread_mutex_lock(&existing_q->lock); // Maybe we don't need to lock if we are destroying?
  pthread_mutex_destroy(&existing_q->lock);
  QueueItem *cur_item = existing_q->head;
  QueueItem *next_item = NULL;
  while(cur_item != NULL){
    next_item = cur_item->next;
    free(cur_item);
    cur_item = next_item;
  }
  free(existing_q);
}

int queue_add(Queue *existing_q, QueueItem *new_item){
  pthread_mutex_lock(&existing_q->lock);
  if(!existing_q->tail && !existing_q->head){
    existing_q->tail = new_item;
    existing_q->head = new_item;
  }
  existing_q->tail->next = new_item;
  sleep(4);
  pthread_mutex_unlock(&existing_q->lock);
  return 0;
}

QueueItem *queue_pop(Queue *existing_q){
  pthread_mutex_lock(&existing_q->lock);
  QueueItem *ret = existing_q->head;
  existing_q->head = existing_q->head->next;
  // Check to see if tail should be set to Null
  if(!existing_q->head){
    existing_q->tail = NULL;
  }
  pthread_mutex_unlock(&existing_q->lock);
  return ret;
}

int queue_add_int(Queue *existing_q, int *i){
  QueueItem *new_item = malloc(sizeof(QueueItem));
  new_item->item = (void *)i;
  return queue_add(existing_q, new_item);
}

int *queue_pop_int(Queue *existing_q){
  QueueItem *popped_item = queue_pop(existing_q);
  int *ret = (int *)popped_item->item;
  free(popped_item);
  return ret;
}

int queue_add_tx(Queue *existing_q, Transaction *tx){
  QueueItem *new_item = malloc(sizeof(Transaction));
  new_item->item = (void *)tx;
  return queue_add(existing_q, new_item);
}

Transaction *queue_pop_tx(Queue *existing_q){
  QueueItem *popped_item = queue_pop(existing_q);
  Transaction *ret = (Transaction *)popped_item->item;
  free(popped_item);
  return ret;
}

int queue_add_block(Queue *existing_q, Block *block){
  QueueItem *new_item = malloc(sizeof(Block));
  new_item->item = (void *)block;
  return queue_add(existing_q, new_item);
}

Block *queue_pop_block(Queue *existing_q){
  QueueItem *popped_item = queue_pop(existing_q);
  Block *ret = (Block *)popped_item->item;
  free(popped_item);
  return ret;
}