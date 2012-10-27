/******************************************************************************
   main.c

   Testing multithreaded FIFO queue

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"

struct add_nodes_data{
  char *mark;
  int count;
  List *l;
};

struct remove_nodes_data{
  int count;
  List *l;
};

// FIFO list;
List *fifo;

void addNodes(void *data){
  struct add_nodes_data *dat = data;
  int i;
  for(i=0; i<dat->count; i++){
    char label[50];
    sprintf(label,"%s%d",dat->mark,i);
    printf("Added element %s\n",label);
    list_add(dat->l, node_new_str(label));
  }
}

void removeNodes(void *data){
  struct remove_nodes_data *dat = data;
  int i;
  Node *n;
  for(i=0; i<dat->count; i++){
    n = list_remove(dat->l);
    if(n != NULL)
      printf("Removed element %s\n",n->elm);
    else 
      printf("Tried to remowe element from empty list\n");
  }
}

int main(int argc, char* argv[])
{
  fifo = list_new();

  struct add_nodes_data add_data;
  add_data.mark =  "s";
  add_data.count = 14;
  add_data.l = fifo;
  addNodes(&add_data);
  
  struct remove_nodes_data rem_data;
  rem_data.count = 12;
  rem_data.l = fifo;
  removeNodes(&rem_data);

  return 0;
}


