#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "pthread.h"
#include "queue.h"
#include "base_tx.h"
#include "base_block.h"
#include <unistd.h>

Queue *queue_init(){
  Queue *new_q = malloc(sizeof(Queue));
  new_q->head = NULL;
  new_q->tail = NULL;
  //INitialize sepmaphores and lock
  sem_init(&new_q->sem_len, 0, 0);
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

int queue_add_void(Queue *existing_q, void *new_item){
  pthread_mutex_lock(&existing_q->lock);
  QueueItem *new_entry = malloc(sizeof(QueueItem));
  new_entry->item = new_item;
  new_entry->next = NULL;
  if(!existing_q->tail && !existing_q->head){
    existing_q->tail = new_entry;
    existing_q->head = new_entry;
  }
  else{
    existing_q->tail->next = new_entry;
    existing_q->tail = new_entry;
  }
  pthread_mutex_unlock(&existing_q->lock);
  int ret = sem_post(&existing_q->sem_len); 
  return 0;
}

void *queue_pop_void(Queue *existing_q){
  sem_wait(&existing_q->sem_len);
  pthread_mutex_lock(&existing_q->lock);
  if(!existing_q->head){
    fprintf(stderr, "Popping off empty Queue");
    exit(1);
  }

  QueueItem *popped_item = existing_q->head;
  existing_q->head = existing_q->head->next;
  // Check to see if tail should be set to Null
  if(!existing_q->head){
    existing_q->tail = NULL;
  }
  void *ret = popped_item->item;
  free(popped_item);
  pthread_mutex_unlock(&existing_q->lock);
  return ret;
}
