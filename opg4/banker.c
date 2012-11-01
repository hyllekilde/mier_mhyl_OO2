#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
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
pthread_mutexattr_t attr;
/* Random sleep function */
void Sleep(float wait_time_ms)
{
  // add randomness
  wait_time_ms = ((float)rand())*wait_time_ms / (float)RAND_MAX;
  usleep((int) (wait_time_ms * 1e3f)); // convert from ms to us
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

void free_matrix(int** matrix){
  free(*matrix);
  free(matrix);
}

//Compare the two vectors. Return 1 if all elements in first is smaller or equal to elements in second, or else return 0
int all_less_equal(int *vec1, int *vec2, int l){
  int i;
  for(i=0; i<l;i++)
    if(vec1[i]>vec2[i]) return 0;
  return 1;
}

/* Is this state safe according to bankers algorithm? Returns 1 if yes, 0 if no */
int is_safe_bankers(){
  //Initialize variables
  int i;
  int j;
  int *work;
  int *finish;
  int res = 1;
  work =  malloc(n*sizeof(int));
  memcpy(work, s->available, n*sizeof(int));
  finish =  malloc(m*sizeof(int));
  for(i=0; i<m; i++) finish[i] = 0;

  //Run bankers algorithm
  i = find_banker_i(work,finish);
  while(i != -1){
    finish[i] = 1;
    for(j=0; j<n; j++)
      work[j] += s->allocation[i][j];
    i = find_banker_i(work,finish);
  }

  //Read the result and return
  for(i=0; i<m; i++)
    if(finish[i] == 0) res = 0;

  free(finish);
  free(work);

  return res;
}

/* Return an index that satisfies condition 2 in bankers 
   algorithm, if no such index is found, return -1 */
int find_banker_i(int *work, int *finish){
  int i;
  int j;
  for(i=0; i<m; i++){
    if(finish[i] == 0 && all_less_equal(s->need[i],work,n)==1) return i;
  }
  return -1;
}

/* Allocate resources in request for process i, only if it 
   results in a safe state and return 1, else return 0 */
int resource_request(int i, int *request)
{
  pthread_mutex_lock(&state_mutex);

  /*if(!cmpvector(request,s->need[i],n)){
    printf("Request contained more resources than allowed for the process\n");
    }*/

  //if the request resources is less than or equal to the available resources, allocate resources
  if(all_less_equal(request,s->available,n) == 1){
    int j;
    for(j=0;j<n;j++){
      s->available[j] -= request[j];
      s->allocation[i][j] += request[j];
      s->need[i][j] -= request[j];
    }
    //If state is safe - Return 1 and keep the resources allocated
    if(is_safe_bankers() == 1){
      pthread_mutex_unlock(&state_mutex);
      return 1;
    }else{ //Undo the tentative allocation
      s->available[j] += request[j];
      s->allocation[i][j] -= request[j];
      s->need[i][j] += request[j];
    }
  }

  pthread_mutex_unlock(&state_mutex);
  return 0;
}

/* Release the resources in request for process i */
void resource_release(int i, int *request)
{
  pthread_mutex_lock(&state_mutex);

  int j;
  for(j=0;j<n;j++){
    s->available[j] += request[j];
    s->allocation[i][j] -= request[j];
    s->need[i][j] += request[j];
  }
  pthread_mutex_unlock(&state_mutex);
}

/* Generate a request vector */
void generate_request(int i, int *request)
{
  int j, sum = 0;
  while (!sum) {
    for (j = 0;j < n; j++) {
      request[j] = (double)s->need[i][j] * ((double)rand()) / (double)RAND_MAX;
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
      request[j] = (double)s->need[i][j]*((double)rand())/ (double)RAND_MAX;
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

int main(int argc, char* argv[])
{
  pthread_mutexattr_init(&attr);
  pthread_mutex_init(&state_mutex,&attr);
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

  printf("Initial state is safe. Running.\n"); //If state was unsafe, we would have exited!

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
  free_matrix(s->max);
  free_matrix(s->allocation);
  free_matrix(s->need);
  free(s);

  print("Succesfully finished");
}
