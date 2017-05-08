#include "SortedList.h"
#include <string.h>


void SortedList_insert(SortedList_t *list, SortedListElement_t *element) 
{
  if (list == NULL || element == NULL)
    {
      return;
    }

  SortedListElement_t* curr = list->next;


  while (curr->next != NULL)
    {
      if (element->key < curr->key)
	break;
      curr = curr->next;
    }

  element->next = curr;
  element->prev = curr->prev;
  curr->prev->next = element;
  curr->prev = element;

}

int SortedList_delete( SortedListElement_t *element)
{
  if (element == NULL)
    return 1;

  if (element->next->prev == element->prev->next)
    {
      element->prev->next = element->next;
      element->next->prev = element->prev;
      element->next = NULL;
      element->prev = NULL;
      return 0;
    }
  return 1;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
  if (list == NULL || key == NULL)
    return NULL;
  SortedListElement_t* curr = list->next;
  while (curr != NULL)
    {
      if (strcmp(curr->key,key) == 0)
	return curr;
      curr = curr->next;
    }

  return NULL;

}

int SortedList_length(SortedList_t *list)
{
  if (list == NULL)
    return 0;
  int count = 0;
  SortedListElement_t* curr = list->next;
  while (curr != NULL)
    {
      count++;
      curr = curr->next;
    }
  return count;
}

