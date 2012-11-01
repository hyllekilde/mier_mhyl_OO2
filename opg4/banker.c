#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct state {
  int *resource;
  int *available;
  int **max;
  int **allocation;
  int **need;
} State;

// Global variables
int m, n;
State *s = NULL;

// Mutex for access to state.
pthread_mutex_t state_mutex;

/* Random sleep function */
void Sleep(float wait_time_ms)
{
  // add randomness
  wait_time_ms = ((float)rand())*wait_time_ms / (float)RAND_MAX;
  usleep((int) (wait_time_ms * 1e3f)); // convert from ms to us
}

/* Allocate resources in request for process i, only if it 
   results in a safe state and return 1, else return 0 */
int resource_request(int i, int *request)
{
  return 0;
}

/* Release the resources in request for process i */
void resource_release(int i, int *request)
{
}

/* Generate a request vector */
void generate_request(int i, int *request)
{
  int j, sum = 0;
  while (!sum) {
    for (j = 0;j < n; j++) {
      request[j] = s->need[i][j] * ((double)rand())/ (double)RAND_MAX;
      sum += request[j];
    }
  }
  printf("Process %d: Requesting resources.\n",i);
}

/* Generate a release vector */
void generate_release(int i, int *request)
{
  int j, sum = 0;
  while (!sum) {
    for (j = 0;j < n; j++) {
      request[j] = s->allocation[i][j] * ((double)rand())/ (double)RAND_MAX;
      sum += request[j];
    }
  }
  printf("Process %d: Releasing resources.\n",i);
}

/* Threads starts here */
void *process_thread(void *param)
{
  /* Process number */
  int i = (int) (long) param, j; //TODO: What is j?!?!
  /* Allocate request vector */
  int *request = malloc(n*sizeof(int));
  while (1) {
    /* Generate request */
    generate_request(i, request);
    while (!resource_request(i, request)) {
      /* Wait */
      Sleep(100);
    }
    /* Generate release */
    generate_release(i, request);
    /* Release resources */
    resource_release(i, request);
    /* Wait */
    Sleep(1000);
  }
  free(request);
}

int** allocate_matrix(int m, int n){
  int** matrix = malloc(m * sizeof(int*));
  int i;
  for(i=0; i<m; i++){
    matrix[i] = malloc(n * sizeof(int));
  }
  return matrix;
}

void print_vector(int* v, int n){
  int i;
  printf("[");
  for(i=0; i<n; i++)
    printf("%2d ",v[i]);
  printf("]\n");
}

void print_matrix(int** ma, int m, int n){
  int i;
  for(i=0; i<m; i++){
    print_vector(ma[i],n);
  }
}

int main(int argc, char* argv[])
{
  /* Get size of current state as input */
  int i, j; //Initialize counting variables
  scanf("%d", &m); //Read m (number of processes)
  scanf("%d", &n); //Read n (number of resources)

  /* Allocate memory for state */
  s = (State *) malloc(sizeof(State));
  s->resource = (int *) malloc(n*sizeof(int));
  s->available = (int *) malloc(n*sizeof(int));
  s->max = allocate_matrix(m,n);
  s->allocation = allocate_matrix(m,n);
  s->need = allocate_matrix(m,n);
  if (s == NULL) { printf("\nYou need to allocate memory for the state!\n"); exit(0); };

  /* Get current state as input */
  for(i = 0; i < n; i++)
    scanf("%d", &s->resource[i]);
  
  for(i = 0;i < m; i++)
    for(j = 0;j < n; j++)
      scanf("%d", &s->max[i][j]);
  
  for(i = 0; i < m; i++)
    for(j = 0; j < n; j++) {
      scanf("%d", &s->allocation[i][j]);
    }

  //Print input
  printf("##INPUT##\n");
  printf("Processes: %d\n",m);
  printf("Resources: %d\n",n);
  printf("Resource vector:\n");
  print_vector(s->resource,n);
  printf("Max matrix:\n");
  print_matrix(s->max,m,n);
  printf("Allocation matrix:\n");
  print_matrix(s->allocation,m,n);
  printf("\n");

  /* Calcuate the need matrix */
  for(i = 0; i < m; i++)
    for(j = 0; j < n; j++)
      s->need[i][j] = s->max[i][j]-s->allocation[i][j];

  /* Calcuate the availability vector */
  for(i = 0; i < n; i++) {
    int sum = 0;
    for(j = 0; j < m; j++)
      sum += s->allocation[j][i];
    s->available[i] = s->resource[i] - sum;
  }

  //Print calculations
  printf("##CALCULATIONS##\n");
  printf("Need matrix:\n");
  print_matrix(s->need,m,n);
  printf("Available vector:\n");
  print_vector(s->available,n);

  /* If initial state is unsafe then terminate with error */
  //TODO: Implement with bankers algorithm...

  /* Seed the random number generator */
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srand(tv.tv_usec);
  
  /* Create m threads */
  pthread_t *tid = malloc(m*sizeof(pthread_t));
  for (i = 0; i < m; i++)
    pthread_create(&tid[i], NULL, process_thread, (void *) (long) i);
  
  /* Wait for threads to finish */
  pthread_exit(0);
  free(tid);

  /* Free state memory */
  //TODO: Should free be more extensive
  free(s->resource);
  free(s->available);
  free(s->max);
  free(s->allocation);
  free(s->need);
  free(s);
}
