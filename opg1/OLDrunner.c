#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

double sum; /* this data is shared by the threads */
void *runner(void *param); /* the thread */

int main(int argc, char* argv[])
{
  pthread_t tid; /* the thread identifier */
  pthread_attr_t attr; /* set of thread attributes */
  clock_t start, finish;

  if (argc !=2) {
    fprintf(stderr,"usage: a.out <inter value>\n");
    return -1;
  }
  if (atoi(argv[1]) < 0) {
    fprintf(stderr, "%d must be >= 0\n",atoi(argv[1]));
    return -1;
  }

  start = clock();

  /* get the default attributes */
  pthread_attr_init(&attr);
  /* create the thread */
  pthread_create(&tid,&attr,runner,argv[1]);
  /* wait for the thread to exit */
  pthread_join(tid,NULL);

  finish = clock();

  printf("sum = %lf in %lf seconds\n",sum,(double)(finish-start)/CLOCKS_PER_SEC);
}

/* The thread will begin control in this function */
void *runner(void *param)
{
  int i, upper = atoi(param);
  sum = 0;

  for(i=1; i <= upper; i++)
    sum +=sqrt(i);

  pthread_exit(0);
}