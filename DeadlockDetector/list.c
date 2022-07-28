#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"

struct list {
  int pid;
  struct list *next;
};

struct list *newlist(int value){
  struct list *head = (struct list *) malloc(sizeof(struct list));
  assert(head);
  head->pid = value;
  head->next = NULL;
  return head;
}

struct list *addToBeginning(struct list *list, int value){
  struct list *head = (struct list *) malloc(sizeof(struct list));
  assert(head);
  head -> pid = value;
  head -> next = list;
  return head;
}

int findTerminateNode(struct list* process_list, int starting_point){
    int min = process_list->pid;
    while(process_list->pid != starting_point){
      if(process_list->pid < min){
          min = process_list->pid;
      }
      
      process_list = process_list->next;
    }
    if(process_list->pid < min){
        min = process_list->pid;
    }
    return min;
}

void freeList(struct list *list){
  struct list *next;
  /* Iterate through list until the end of the list (NULL) is reached. */
  for(next = list; list != NULL; list = next){
    /* Store next pointer before we free list's space. */
    next = list->next;
    free(list);
  }
}

