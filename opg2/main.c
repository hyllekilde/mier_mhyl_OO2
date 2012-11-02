/******************************************************************************
   main.c

   Testing multithreaded FIFO queue

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"

//A struct to contain data about the node to be added
struct add_nodes_data{
  char *mark;
  int count;
  List *l;
};

//A struct to contain data about the list to remove from
struct remove_nodes_data{
  int count;
  List *l;
};

// FIFO list;
List *fifo;

//Add nodes to the list
void *addnodes(void *data){
  struct add_nodes_data *dat = data;
  int i;
  for(i=0; i<dat->count; i++){
    char label[50];
    sprintf(label,"%s%d",dat->mark,i); //Construct a label to identify the data and where it's coming from
    printf("Added element %s\n",label);
    list_add(dat->l, node_new_str(label)); //Add node
    sleep(1);
  }
}

//Remove nodes from the list
void *removenodes(void *data){
  struct remove_nodes_data *dat = data;
  int i;
  Node *n;
  for(i=0; i<dat->count; i++){
    n = list_remove(dat->l); //remove node
    if(n != NULL)
      printf("Removed element %s\n",n->elm);
    else 
      printf("Tried to remove element from empty list\n");
    sleep(2);
  }
}

int main(int argc, char* argv[])
{
  //Initialize a new list
  fifo = list_new();

  //Set data and create thread 1
  struct add_nodes_data add_data;
  pthread_t tid1;
  add_data.mark = "s";
  add_data.count = 10;
  add_data.l = fifo;
  pthread_attr_t attr1;
  pthread_attr_init(&attr1);
  pthread_create(&tid1, &attr1, addnodes, &add_data);

  //Set data and create thread 2
  struct remove_nodes_data rem_data;
  pthread_t tid2;
  rem_data.count = 12;
  rem_data.l = fifo;
  pthread_attr_t attr2;
  pthread_attr_init(&attr2);
  pthread_create(&tid2, &attr2, removenodes, &rem_data);

  //Join threads (makes the program wait for them to terminate)
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);  

  return 0;
}
