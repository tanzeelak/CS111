#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <string.h>


#define print_err() do {if (errno) {fprintf(stderr, "error %s", strerr(errno)); exit(1);}} while(0)


pthread_mutex_t count_mutex;
long long count;
int threadNum = 1;
int iterNum = 1;
int opt_yield = 0;
int testAndSet = 0;
char syncopt = NULL;
int yieldFlag = 0;
char tag[20];

void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if (opt_yield)
    sched_yield();
  *pointer = sum;
}

void add_c(long long *pointer, long long value) {
  long long oldValue;
  long long sum;
  do {
    oldValue = *pointer;
    sum = oldValue + value;
    if (opt_yield)
      sched_yield();
  }while(__sync_val_compare_and_swap(pointer, oldValue, sum) != oldValue);

}

void* threadAdd(void* ptr)
{
  int i;
  long long* counter = (long long*)ptr;

  for (i = 0; i < iterNum; i++)
    {

      if (syncopt == 'm')
	{

	  pthread_mutex_lock(&count_mutex);
	  add(counter, 1);
	  add(counter, -1);
	  pthread_mutex_unlock(&count_mutex);
	}	
      else if (syncopt == 's')
	{
  	  while(__sync_lock_test_and_set(&testAndSet, 1));
	  add(counter, 1);
	  add(counter, -1);
	  __sync_lock_release(&testAndSet);
	}
      else if (syncopt == 'c')
	{
	  add_c(counter, 1);
	  add_c(counter, -1);
	}
      else 
	{
	  add(counter, 1);
	  add(counter, -1);
	}
      
    }

}

void determineTag(void)
{
    if (yieldFlag && syncopt == 'm')
	strcpy(tag,"add-yield-m");
    else if (yieldFlag && syncopt == 's')
	strcpy(tag,"add-yield-s");
    else if (yieldFlag && syncopt == 'c')
	strcpy(tag,"add-yield-c");
    else if (yieldFlag)
	strcpy(tag,"add-yield-none");
    else if (syncopt == 'm')
	strcpy(tag,"add-m");
    else if (syncopt == 's')
	strcpy(tag,"add-s");
    else if (syncopt == 'c')
	strcpy(tag,"add-s");
    else 
	strcpy(tag,"add-none");

}

int main(int argc, char *argv[])
{

    int optParse = 0;
    int threadFlag = 0;
    int iterFlag = 0;
    int syncFlag = 0;
    char* threadopt = NULL;
    char* iteropt = NULL;
    char* yieldopt = NULL;
    struct timespec start, end;
    //    char* tag = NULL;
    int i;
    int rc;
    pthread_attr_t attr;
    void *status;
    

    static struct option long_options[] = {
      {"threads", required_argument, 0, 't'},
      {"iterations", required_argument, 0, 'i'},
      {"yield", no_argument, 0, 'y'},
      {"sync", required_argument, 0, 's'},
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
	case 's':
	  syncFlag = 1;
	  syncopt = optarg[0];
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
    if (syncFlag)
      {
      }
    determineTag();
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
    fprintf(stdout, "%s,%i,%i,%i,%lli,%lli,%lli\n", tag,threadNum, iterNum, numOp, runTime, aveTime, count);

}
