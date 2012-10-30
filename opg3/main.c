/******************************************************************************
  main.c

  Testing multithreaded FIFO queue

 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "list.h"

//A struct to contain data for a producer or consumer thread
struct thread_data{
  int num;
  List *l;
};

int buffermax;
int producemax;
int produced;
pthread_mutex_t produced_lock;
sem_t empty;
sem_t full;

/*Random Sleep*/
void randsleepms(float wait_time_ms){
  wait_time_ms = ((float)rand())*wait_time_ms / (float) RAND_MAX;
  usleep((int) (wait_time_ms * 1e3f)); //Convert from ms to us
}

void *consume(void *data){
  struct thread_data *dat = data;
  while(1==1){
    Node *n;
    //sem_wait(&full);
    n = list_remove(dat->l);
    //sem_post(&empty);
    if(n!=NULL) 
      printf("Consumed: \"%s\"\n",n->elm);
    else
      printf("Tried to remove from empty list\n");
    randsleepms(500);
  }
}

void *produce(void *data){
  struct thread_data *dat = data;
  pthread_mutex_lock(&produced_lock);
  int count_produced = produced;
  produced++;
  pthread_mutex_unlock(&produced_lock);
  while(count_produced < producemax){
    //sem_wait(&empty);
    printf("Producing (tid: %d): %d - %d\n",(unsigned int) pthread_self(),dat->num,count_produced);
    char nodedata[30];
    sprintf(nodedata, "Element %d from thread %d", count_produced, dat->num);
    list_add(dat->l, node_new_str(nodedata));
    //sem_post(&full);
    randsleepms(500);
    pthread_mutex_lock(&produced_lock);
    count_produced = produced;
    produced++;
    pthread_mutex_unlock(&produced_lock);
  }
}

int main(int argc, char* argv[])
{
  //TODO: We should make som sanity checks on the input value
  buffermax = atoi(argv[3]);
  sem_init(&empty, 0, buffermax);
  sem_init(&full, 0, 0);
  produced = 0;
  producemax = atoi(argv[4]);
  int producers = atoi(argv[1]);  
  int consumers = atoi(argv[2]);

  pthread_mutexattr_t muattr;
  pthread_mutexattr_init(&muattr);
  pthread_mutex_init(&produced_lock, &muattr);

  int i;

  List *fifo = list_new(); //Initialize a list to work on

  pthread_t tids[producers+consumers];

  for(i = 0; i < producers; i++){
    struct thread_data *data;
    data = malloc(sizeof(struct thread_data));
    data->num = i;
    data->l = fifo;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tids[i], &attr, produce, data);
  }

  for(i = 0; i < consumers; i++){
    struct thread_data *data;
    data = malloc(sizeof(struct thread_data));
    pthread_t tid;
    data->num = i;
    data->l = fifo;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tids[producers+i], &attr, consume, data);
  }

  for(i = 0; i < (producers+consumers); i++){
    pthread_join(tids[i],NULL);
  }


  /*fifo = list_new();

  //Set data and create thread 1
  struct add_nodes_data add_data;
  pthread_t tid1;
  add_data.mark = "s";
  add_data.count = 14;
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

  //Join threads
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);  

  return 0;'*/
}

