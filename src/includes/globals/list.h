#pragma once

#include <stdlib.h>

typedef struct {
  void **data;
  size_t len;
  size_t total_len;
  size_t element_size;
} List;

List *build_list(size_t element_size);
void list_append(List *list, void *entry);
void *list_pop(List *list);
void free_list(List *list);
