#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

pthread_mutex_t sum_lock;

struct thread_data{
  int start;
  int end;
};

long double sum; /* this data is shared by the threads */
void *runner(void *threadarg); /* the thread */

int main(int argc, char* argv[])
{
  int amount = atoi(argv[1]);
  int nThreads = atoi(argv[2]);
  int interval = amount/nThreads;
  int i;
  pthread_t tid[nThreads]; /* the thread identifier */

  //Check input
  if (argc !=3){
    fprintf(stderr,"usage: a.out #sum_to #num_threads\n");
    return -1;
  }
  if (amount < 1){
    fprintf(stderr, "first argument must be > 0\n");
    return -1;
  }
  if (nThreads < 1){
    fprintf(stderr, "second argument must be > 0\n");
    return -1;
  }
  if (amount % nThreads > 0){
    fprintf(stderr, "Secound argument must be a divisor for first argument!\n");
    return -1;
  }

  //Print input for convenience
  printf("sumNum: %d\n",amount);
  printf("tNumber: %d\n",nThreads);
  printf("interval: %d\n",interval);

  //Initialize the sum to 0.0
  sum = 0.0;

  //Calculate the sum with nThreads threads
  for(i = 0; i < nThreads; i++){
    struct thread_data *thread_data; //Struct to hold thread data as single argument
    thread_data = malloc(sizeof(struct thread_data)); //Malloc the thread data so it is not overridden
    //Calculate the start and end of calculation for this thread
    thread_data->start = (i*interval)+1;
    thread_data->end = (1+i)*interval;
    pthread_attr_t attr; 
    pthread_attr_init(&attr);
    pthread_create(&tid[i],&attr,runner,thread_data); //Create the thread
  }

  //Catch the results from threads and calculate total sum
  void  *res;
  for(i = 0; i < nThreads; i++){
    pthread_join(tid[i],&res); //Return result in res
    sum += *(long double *) res; //Add res to sum (must be casted)
    free(res); //Res was malloc'ed in thread and can now be freed
  }

  printf("sum = %3lf\n",(double)sum);
  exit(0);
}

/* The thread will begin control in this function */
void *runner(void *threadarg)
{
  struct thread_data *thread_data = threadarg;
  long double *locSum = malloc(sizeof(long double)); //Setup a local sum
  *locSum = 0.0; //Initialize the local sum to 0

  //Calculate the sum in the given interval
  int i;
  for(i=thread_data->start; i <= thread_data->end; i++){
    *locSum +=sqrt(i);
  }
  
  free(threadarg); //This resource is now used and can be freed
  pthread_exit((void *) locSum); //Return the partial result
}
