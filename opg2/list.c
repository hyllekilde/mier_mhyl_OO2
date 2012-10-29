/******************************************************************************
   list.c

   Implementation of simple linked list defined in list.h.

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "list.h"

pthread_mutex_t mutex;
pthread_mutexattr_t attr;
/* list_new: return a new list structure */
List *list_new(void)
{
  List *l;
  pthread_mutexattr_init(&attr);
  
  l = (List *) malloc(sizeof(List));
  l->len = 0;

  /* insert root element which should never be removed */
  //TODO: Why not use node_new function?
  l->first = l->last = (Node *) malloc(sizeof(Node));
  l->first->elm = NULL;
  l->first->next = NULL;
 
  pthread_mutex_init(&mutex, &attr); //Initialize a lock for the specified list
  return l;
}

/* list_add: add node n to list l as the last element */
void list_add(List *l, Node *n)
{
  pthread_mutex_lock(&mutex);
  l->last->next = n; //Make the last element point to the new node, which makes the new node the last node
  l->last = n; //Tell the list about the new last node
  l->len = l->len+1; //Increment the length of the list
  pthread_mutex_unlock(&mutex);
}

/* list_remove: remove and return the first (non-root) element from list l */
Node *list_remove(List *l)
{
  pthread_mutex_lock(&mutex);
  Node *n;
  if((l->len)<1){ //If the list is empty
    pthread_mutex_unlock(&mutex);
    return NULL;
  }else if((l->len)==1){ //If the list contains 1 node
    n = l->first->next;
    l->first->next = NULL;
    l->last = l->first;
  }else{ //If the list contains more than one node
  n = l->first->next; //Get the firts node we want to remove (next node after the first, which is the empty node)
  l->first->next = n->next; //Remove the node by pointing the first node to the next node after the node that we are removing
  }
  l->len = l->len-1; //Decrement the length of the list
  pthread_mutex_unlock(&mutex);
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
