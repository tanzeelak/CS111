#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define print_err() do {if (errno) {fprintf(stderr, "error %s", strerr(errno)); exit(1);}} while(0)


pthread_mutex_t count_mutex;
long long count;
int threadNum = 1;
int iterNum = 1;
int opt_yield = 0;

void addNone(long long *pointer, long long value) {
  long long sum = *pointer + value;
  *pointer = sum;
}

void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if (opt_yield)
    sched_yield();
  *pointer = sum;
}

/*
void fakeAdd(struct Data *datPass)
{
  int i;

  for (i = 0; i < datPass->iterNum; i++)
    {
      ++(*(datPass->ptr));
      if (opt_yield)
	sched_yield();
    }
  for (i = 0; i < datPass->iterNum; i++)
    {    
      --(*(datPass->ptr));
      if (opt_yield)
	sched_yield();
    }
 
}
*/
void* threadAdd(void* ptr)
{
  int i;
  long long* counter = (long long*)ptr;
  for (i = 0; i < iterNum; i++)
    {
      add(counter, 1);
      add(counter, -1);
    }

}

int main(int argc, char *argv[])
{

    int optParse = 0;
    int threadFlag = 0;
    int iterFlag = 0;
    int yieldFlag = 0;
    char* threadopt = NULL;
    char* iteropt = NULL;
    char* yieldopt = NULL;

    struct timespec start, end;

    int i;
    int rc;
    pthread_attr_t attr;
    void *status;
   

    static struct option long_options[] = {
      {"threads", required_argument, 0, 't'},
      {"iterations", required_argument, 0, 'i'},
      {"yield", no_argument, 0, 'y'},
      {0,0,0,0}
    };

    int option_index = 0;
    while((optParse = getopt_long(argc, argv, "i:o:sc:", long_options, &option_index)) != -1){
      switch (optParse)
        {
        case 't':
          threadFlag = 1;
          threadopt = optarg;
          break;
        case 'i':
          iterFlag = 1;
          iteropt = optarg;
          break;
	case 'y':
	  yieldFlag = 1;
	  break;
        default:
          fprintf(stderr, "Proper usage of options: --port=portnum, --encrypt=filename\n");
          exit(1);
        }
    }

    if (threadFlag)
      {
	threadNum = atoi(threadopt);
      }
    if (iterFlag)
      {
	iterNum = atoi(iteropt);
      }
    if (yieldFlag)
      {
	opt_yield = 1;
      }

    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_t *threads = malloc(threadNum * sizeof(pthread_t));

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (i = 0; i < threadNum; i++)
      {
	rc = pthread_create(&threads[i], &attr, threadAdd, &count);
	if (rc) {
	  fprintf(stderr, "ERROR: return code from pthread_create():%d\n", rc);
	  exit(-1);
	}
	
      }

    for (i = 0; i < threadNum; i++)
      {
	rc = pthread_join(threads[i], &status);
	if (rc) {
	  fprintf(stderr, "ERROR: return code from pthread_join() is %d\n", rc);
	  exit(-1);
	}
      }

    clock_gettime(CLOCK_MONOTONIC, &end);


    
    long long numOp = threadNum * iterNum * 2;
    long long runTime = end.tv_nsec - start.tv_nsec;
    long long aveTime = runTime/numOp;
    fprintf(stdout, "add-none,%i,%i,%i,%lli,%lli,%lli\n", threadNum, iterNum, numOp, runTime, aveTime, count);

}