#include<stdio.h>
#include<stdlib.h>
#include<string.h>
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

/* Is this state safe according to bankers algorithm? Returns 1 if yes, 0 if no */
int is_safe_bankers(){
  //Initialize variables
  int i;
  int j;
  int *work;
  int *finish;
  work = malloc(n*sizeof(int));
  memcpy(work, s->available, n*sizeof(int));
  finish = malloc(m*sizeof(int));
  for(i=0; i<m; i++) finish[i] = 0;

  //Run bankers algorithm
  i = find_banker_i(work,finish);
  while(i != -1){
    finish[i] = 1;
    for(j=0; j<n; j++)
      work[j] += s->allocation[i][j];  
    //print_vector(work);
    i = find_banker_i(work,finish);
  }

  //Read the result and return
  for(i=0; i<m; i++){
   if(finish[i] == 0) return 0;
  }
  return 1;
}

/* Return an index that satisfies condition 2 in bankers 
algorithm, if no such index is found, return -1 */
int find_banker_i(int *work, int *finish){
  int i;
  int j;
  int need_cond;
  for(i=0; i<m; i++){
    need_cond = 1;
    for(j=0; j<n; j++)
      if(s->need[i][j] >= work[j]) need_cond = 0;
    if(finish[i] == 0 && need_cond) return i;
  }
  return -1;
}

/* Allocate resources in request for process i, only if it 
   results in a safe state and return 1, else return 0 */
int resource_request(int i, int *request)
{
  //TODO: Implement with bankers algorithm
  return 0;
}

/* Release the resources in request for process i */
void resource_release(int i, int *request)
{
  //TODO: Implement this
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

void print_vector(int* v){
  int i;
  printf("[");
  for(i=0; i<n; i++)
    printf("%2d ",v[i]);
  printf("]\n");
}

void print_matrix(int** ma){
  int i;
  for(i=0; i<m; i++){
    print_vector(ma[i]);
  }
}

int main(int argc, char* argv[])
{ 
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
  print_vector(s->resource);
  printf("Max matrix:\n");
  print_matrix(s->max);
  printf("Allocation matrix:\n");
  print_matrix(s->allocation);
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
  print_matrix(s->need);
  printf("Available vector:\n");
  print_vector(s->available);

  printf("Checking to see if initial state is safe:\n");
  /* If initial state is unsafe then terminate with error */
  if(is_safe_bankers()<1){
    printf("Initial state is unsafe or could not be determined. Exiting!\n");
    exit(-1);
  }

  printf("Inital state is safe. Running.\n"); //If state was unsafe, we would have exited!

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
  free(s->resource);
  free(s->available);
  free(s->max);
  free(s->allocation);
  free(s->need);
  free(s);
}
