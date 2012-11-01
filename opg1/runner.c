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
  clock_t start, finish;
  int amount = atoi(argv[1]);
  int nThreads = atoi(argv[2]);
  int interval = amount/nThreads;
  int i;
  pthread_t tid[nThreads]; /* the thread identifier */

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

  printf("sumNum: %d\n",amount);
  printf("tNumber: %d\n",nThreads);
  printf("interval: %d\n",interval);

  sum = 0.0;

  start = clock();

  for(i = 0; i < nThreads; i++){
    struct thread_data thread_data;
    thread_data.start = (i*interval)+1;
    thread_data.end = (1+i)*interval;
    pthread_attr_t attr; /* set of thread attributes */
    /* get the default attributes */
    pthread_attr_init(&attr);
    /* create the thread */
    pthread_create(&tid[i],&attr,runner,&thread_data);
  }

  /* wait for the threads to exit */
  for(i = 0; i < nThreads; i++){
    pthread_join(tid[i],NULL);
  }
  finish = clock();

  printf("sum = %3lf in %3lf seconds\n",(double)sum,(double)(finish-start)/CLOCKS_PER_SEC);
  exit(0);
}

/* The thread will begin control in this function */
void *runner(void *threadarg)
{
  struct thread_data *thread_data = threadarg;
  long double *locSum = malloc(sizeof(long double));
  *locSum = 0;

  int i;
  for(i=thread_data->start; i <= thread_data->end; i++){
    *locSum +=sqrt(i);
  }
  
  pthread_mutex_lock(&sum_lock);
  sum += *locSum;
  pthread_mutex_unlock(&sum_lock);
  pthread_exit(0);
}
