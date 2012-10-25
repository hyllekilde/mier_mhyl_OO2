/******************************************************************************
   main.c

   Testing multithreaded FIFO queue

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"

// FIFO list;
List *fifo;

void addNodes(char *mark,int count,List *l){
  int i;
  for(i=0; i<count; i++){
    char *label;
    sprintf(label,"%s%d",mark,i);
    list_add(l, node_new_str(label));
  }
}

void removeNodes(int count, List *l){
  int i;
  Node *n;
  for(i=0; i<count; i++){
    n = list_remove(l);
    if(n != NULL)
      printf("Removed element %s\n",n->elm);
    else 
      printf("Tried to remowe element from empty list\n");
  }
}

int main(int argc, char* argv[])
{
  fifo = list_new();

  addNodes("s", 10, fifo);
  removeNodes(15, fifo);

  return 0;
}


