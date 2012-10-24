#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

struct thread_data{
  int start;
  int end;
};

pthread_mutex_t mutexsum;
double sum; /* this data is shared by the threads */
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
  if (atoi(argv[1]) < 0){
    fprintf(stderr, "%d must be >= 0\n",amount);
    return -1;
  }
  if((atoi(argv[2]) % 2) != 0){ //WHY?!
    fprintf(stderr,"The number of threads %d must be an equal number\n",nThreads);
    return -1;
  }

  printf("sumNum: %d\n",amount);
  printf("tNumber: %d\n",nThreads);
  printf("interval: %d\n",interval);

  pthread_mutex_init(&mutexsum, NULL);
  start = clock();

  for(i = 0; i < nThreads; i++){
    struct thread_data thread_data[i];
    thread_data[i].start = (i*interval)+1;
    thread_data[i].end = (1+i)*interval;
    pthread_attr_t attr; /* set of thread attributes */
    /* get the default attributes */
    pthread_attr_init(&attr);
    /* create the thread */
    pthread_create(&tid[i],&attr,runner,&thread_data[i]);
  }

  for(i = 0; i < nThreads; i++){
    /* wait for the thread to exit */
    pthread_join(tid[i],NULL);
  }
  pthread_mutex_destroy(&mutexsum);
  finish = clock();

  printf("sum = %lf in %lf seconds\n",sum,(double)(finish-start)/CLOCKS_PER_SEC);
}

/* The thread will begin control in this function */
void *runner(void *threadarg)
{
  int i;
  double localSum;
  struct thread_data *thread_data = threadarg;

  for(i=thread_data->start; i <= thread_data->end; i++){
    localSum +=sqrt(i);
  }
  
  pthread_mutex_lock(&mutexsum);
  sum+=localSum;
  pthread_mutex_unlock(&mutexsum);

  pthread_exit(0);
}
