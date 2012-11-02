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

int buffermax; //How big is the buffer?
int producemax; //How many items to produce in total?
int produced; //How many items has been produced so far?
pthread_mutex_t produced_lock; //Lock to make sure that produced will not be subject for a race condition
sem_t empty; //Semaphore to count empty spots in buffer
sem_t full; //Semaphore to count number of full/occupied spots in buffer

/*Random Sleep*/
void randsleepms(float wait_time_ms){
  wait_time_ms = ((float)rand())*wait_time_ms / (float) RAND_MAX;
  usleep((int) (wait_time_ms * 1e3f)); //Convert from ms to us
}

/* Thread method for consuming data */
void *consume(void *data){
  struct thread_data *dat = data;
  while(produced <= producemax){ //Consume until all elements have been produced. We might have a race condition, and one extra go on consuming, but that is not a problem with this implementation
    Node *n;
    sem_wait(&full); //Wait until we can remove one full spot -> wait as long as full is zero
    n = list_remove(dat->l); //Remove the element
    sem_post(&empty); //Count empty one up -> Announce that we have removed one element
    if(n!=NULL) 
      printf("Consumed (tid: %d): \"%s\"\n",(unsigned int) pthread_self(),n->elm);
    else
      printf("Tried to remove from empty list\n");
    randsleepms(2000); //Sleep a bit, so we don't stress..
  }
  printf("Thread (pid: %d) successfully assisted in consuming %d elements\n",(unsigned int) pthread_self(),producemax);
}

void *produce(void *data){
  struct thread_data *dat = data;
  //Count produced one up -> This is the one we are gonna produce, so keep the old number for while-condition
  pthread_mutex_lock(&produced_lock);
  int count_produced = produced++;
  pthread_mutex_unlock(&produced_lock);

  //Produce som elements
  while(count_produced < producemax){
    sem_wait(&empty); //Wait until we can fill one empty spot -> wait as long as empty is zero
    char nodedata[30];
    sprintf(nodedata, "Element %d from thread %d", count_produced, dat->num);
    printf("Producing (tid: %d): \"%s\"\n",(unsigned int) pthread_self(),nodedata);
    list_add(dat->l, node_new_str(nodedata)); //Add the element
    sem_post(&full); //Announce that we have filled on spot in the buffer
    randsleepms(2000); //Sleep a bit, no stress..

    //Count produced one up like above
    pthread_mutex_lock(&produced_lock);
    count_produced = produced++;
    pthread_mutex_unlock(&produced_lock);
  }
  printf("Thread (pid: %d) successfully assisted in producing %d elements\n",(unsigned int) pthread_self(),producemax);
}

int main(int argc, char* argv[])
{
  //TODO: We should make som sanity checks on the input value
  
  //Read input and setup vars
  buffermax = atoi(argv[3]);
  sem_init(&empty, 0, buffermax); //Initialize semaphores
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

  /* Seed the random number generator */
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srand(tv.tv_usec);

  //Make som producer threads
  for(i = 0; i < producers; i++){
    struct thread_data *data;
    data = malloc(sizeof(struct thread_data)); //Important to malloc data so it is not overridden in threads!
    data->num = i;
    data->l = fifo;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tids[i], &attr, produce, data);
  }

  //Make som consumer threads
  for(i = 0; i < consumers; i++){
    struct thread_data *data;
    data = malloc(sizeof(struct thread_data)); //Important to malloc data so it is not overridden in threads!
    pthread_t tid;
    data->num = i;
    data->l = fifo;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tids[producers+i], &attr, consume, data);
  }

  //Wait for all threads to finish
  for(i = 0; i < (producers+consumers); i++){
    pthread_join(tids[i],NULL);
  }
  exit(0);
}
