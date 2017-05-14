#include "SortedList.h"
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
  if(list == NULL || element == NULL)
      return;
  SortedListElement_t *p = list->next;
  while(p != list)
    {
      if(strcmp(element->key, p->key) <= 0)
	break;
      p  = p->next;
    }
  if(opt_yield & INSERT_YIELD)
    sched_yield();
  element->next = p;
  element->prev = p->prev;
  p->prev->next = element;
  p->prev = element;
}

int SortedList_delete(SortedListElement_t *element)
{
  if(element == NULL)
      return 1;
  if(element->next->prev == element->prev->next)
    {
      if(opt_yield & DELETE_YIELD)
	sched_yield();
      element->prev->next = element->next;
      element->next->prev = element->prev;
      return 0;
    }
  return 1;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
  if(list == NULL) 
    return NULL;
  SortedListElement_t *p = list->next;
  while(p != list)
    {
      if(strcmp(p->key, key) == 0)
	return p;
      if(opt_yield & LOOKUP_YIELD)
	sched_yield();
      p = p->next;
    }
  return NULL;
}

int SortedList_length(SortedList_t *list)
{
  if(list == NULL)
      return -1;
  int cnt = 0;
  SortedListElement_t *p = list->next;
  while(p != list)
    {
      cnt++;
      if(opt_yield & LOOKUP_YIELD)
	sched_yield();
      p = p->next;
    }
  return cnt;
}
