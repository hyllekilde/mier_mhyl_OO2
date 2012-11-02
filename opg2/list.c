/******************************************************************************
   list.c

   Implementation of simple linked list defined in list.h.


******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "list.h"

/* list_new: return a new list structure */
List *list_new(void)
{
  List *l;
  
  l = (List *) malloc(sizeof(List));

  /* insert root element which should never be removed */
  l->first = l->last = (Node *) malloc(sizeof(Node));
  l->first->elm = NULL;
  l->first->next = NULL;
  
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutex_init(&l->lock, &attr); //Initialize a lock for the specified list
  return l;
}

/* list_add: add node n to list l as the last element */
void list_add(List *l, Node *n)
{
  //Check if values are sane
  if(&l == NULL)
  {
    printf("Illegal pass of NULL as list to list_add!");
    return;
  }
  if(n==NULL) return; //Not allowed to add NULL elements - ignore

  //Lock the list, and perform operation
  pthread_mutex_lock(&l->lock);
  l->last->next = n; //Make the last element point to the new node, which makes the new node the last node
  l->last = n; //Tell the list about the new last node
  l->len++; //Count the length 1 up
  pthread_mutex_unlock(&l->lock);
}

/* list_remove: remove and return the first (non-root) element from list l */
Node *list_remove(List *l)
{
  //Check if values are sane
  if(&l == NULL)
  {
    printf("Illegal pass of NULL as list to list_remove!");
    return NULL;
  }

  //Loock the list and perform operation
  pthread_mutex_lock(&l->lock);
  Node *n = l->first->next; //Get the result, eg. first node after first, NULL if the list is empty
  if(n != NULL)
  {
    l->first->next = n->next; //Make the first point to the next element after the element we remove
    l->len--; //Count the length 1 down
  }
  if(l->last == n) l->last = l->first; //Make sure that we point last to initial element when removing last "real" element
  pthread_mutex_unlock(&l->lock);
  return n;
}

/* node_new: return a new node structure */
Node *node_new(void)
{
  Node *n;
  n = (Node *) malloc(sizeof(Node));
  n->elm = NULL;
  n->next = NULL;
  return n;
}

/* node_new_str: return a new node structure, where elm points to new copy of s */
Node *node_new_str(char *s)
{
  Node *n;
  n = (Node *) malloc(sizeof(Node));
  n->elm = (void *) malloc((strlen(s)+1) * sizeof(char));
  strcpy((char *) n->elm, s);
  n->next = NULL;
  return n;
}
