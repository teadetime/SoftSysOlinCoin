#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "base_tx.h"
#include "base_block.h"

typedef struct QueueItem{
  void *item;
  struct QueueItem *next;
} QueueItem;

typedef struct Queue {
  QueueItem *head;
  QueueItem *tail;
  pthread_mutex_t lock;
  sem_t sem_len;
} Queue;

Queue *queue_init();

void queue_destroy(Queue *exisiting_q);

int queue_add_void(Queue *existing_q, void *new_item);
void *queue_pop_void(Queue *existing_q);