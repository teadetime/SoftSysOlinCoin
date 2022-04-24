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

int queue_add(Queue *existing_q, QueueItem *new_item);

QueueItem *queue_pop(Queue *existing_q);

int queue_add_int(Queue *existing_q, int *i);

int *queue_pop_int(Queue *existing_q);

int queue_add_tx(Queue *existing_q, Transaction *tx);

Transaction *queue_pop_tx(Queue *existing_q);

int queue_add_block(Queue *existing_q, Block *block);

Block *queue_pop_block(Queue *existing_q);