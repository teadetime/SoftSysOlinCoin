#include "list.h"
#include <string.h>

List *build_list(size_t element_size) {
  List *list;

  list = malloc(sizeof(List));
  list->data = malloc(element_size);
  list->len = 0;
  list->total_len = 1;
  list->element_size = element_size;

  return list;
}

void list_append(List *list, void *entry) {
  void *new_data;
  if (list->len >= list->total_len) {
    // Make new array with double the size, then copy data in
    new_data = malloc(sizeof(list->element_size) * list->total_len * 2);
    memcpy(new_data, list->data, list->len * sizeof(list->element_size));

    free(list->data);
    list->data = new_data;
    list->total_len = list->total_len * 2;
  }
  *(list->data + (list->len++ * list->element_size)) = entry;
}

void *list_pop(List *list) {
  if (list->len == 0)
    return NULL;
  return *(list->data + (--list->len * list->element_size));
}

void free_list(List *list) {
  void *element;

  if (list == NULL)
    return;

  if (list->data != NULL) {
    for (size_t i = 0; i < list->len; i++) {
      element = *(list->data + (i * list->element_size));
      if (element != NULL)
        free(element);
    }
    free(list->data);
  }

  free(list);
}
