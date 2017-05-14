#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include "SortedList.h"
#include <signal.h>
#include <errno.h>
#define print_err() do {if (errno) {fprintf(stderr, "error %s", strerr(errno)); exit(1);}} while(0)

pthread_mutex_t count_mutex;
long long count;
int threadNum = 1;
int iterNum = 1;
int opt_yield = 0;
int testAndSet = 0;
char syncopt;
SortedList_t* list; 
SortedListElement_t* elem;
int randSize = 0;
char* randKey = NULL;
int reqNum;
int spin;
int* offsetArr;
char tag[30];
long long ns;
struct timespec start_lock, end_lock;
long long mutex_time = 0;

void sysFailed(char* sysCall, int exitNum)
{
  fprintf(stderr, "%s failed: %s\n", sysCall, strerror(errno));
  exit(exitNum);
}

void initList(void)
{
   list = malloc(sizeof(SortedList_t));
   list->key = NULL;
   list->next = list;
   list->prev = list;
}

void initLocks(void)
{
  if (syncopt == 'm')
    pthread_mutex_init(&count_mutex,NULL);
  if (syncopt == 's')
      spin = 0; 
}

char *randstring(int length) {    
  char *string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";
  size_t stringLen = 26*2+10+7;        
  char *randomString;

  randomString = malloc(sizeof(char) * (length +1));

  if (!randomString) {
    return (char*)0;
  }
  unsigned int key = 0;

  for (int n = 0;n < length;n++) {            
    key = rand() % stringLen;          
    randomString[n] = string[key];
  }

  randomString[length] = '\0';

  return randomString;
}

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

void* listAdd(void* offset)
{


  int i, added, deleted;
  SortedListElement_t *toDel;
  
  //    fprintf(stderr, "offset: %d\n", *(int*)offset);
  for (i = *(int*)offset; i < *(int*)offset+iterNum; i++)
    {
      if (syncopt == 'm')
	{
	  
	  clock_gettime(CLOCK_MONOTONIC, &start_lock);

	  pthread_mutex_lock(&count_mutex);
	  clock_gettime(CLOCK_MONOTONIC, &end_lock);
	  mutex_time += 1000000000L * (end_lock.tv_sec - start_lock.tv_sec) + end_lock.tv_nsec - start_lock.tv_nsec;

	  SortedList_insert(list, &elem[i]);
	  pthread_mutex_unlock(&count_mutex);

	}
      else if (syncopt == 's')
	{

	  clock_gettime(CLOCK_MONOTONIC, &start_lock);

  	  while(__sync_lock_test_and_set(&testAndSet, 1));

	  clock_gettime(CLOCK_MONOTONIC, &end_lock);
	  mutex_time += 1000000000L * (end_lock.tv_sec - start_lock.tv_sec) + end_lock.tv_nsec - start_lock.tv_nsec;

	  SortedList_insert(list, &elem[i]);
	  __sync_lock_release(&testAndSet);
	}
      else
	{
	  SortedList_insert(list, &elem[i]);
	}
    }
  added = SortedList_length(list);
  //  fprintf(stderr, "added length: %d\n", added);

  for (i = *(int*)offset; i < *(int*)offset+iterNum; i++)
    {
      if (syncopt == 'm')
	{
	  clock_gettime(CLOCK_MONOTONIC, &start_lock);

	  pthread_mutex_lock(&count_mutex);
	  clock_gettime(CLOCK_MONOTONIC, &end_lock);
	  mutex_time += 1000000000L * (end_lock.tv_sec - start_lock.tv_sec) + end_lock.tv_nsec - start_lock.tv_nsec;

	  toDel = SortedList_lookup(list, elem[i].key);
	  SortedList_delete(&elem[i]);
	  pthread_mutex_unlock(&count_mutex);

	}
      else if (syncopt == 's')
	{
	  clock_gettime(CLOCK_MONOTONIC, &start_lock);

  	  while(__sync_lock_test_and_set(&testAndSet, 1));
	  clock_gettime(CLOCK_MONOTONIC, &end_lock);
	  mutex_time += 1000000000L * (end_lock.tv_sec - start_lock.tv_sec) + end_lock.tv_nsec - start_lock.tv_nsec;

	  toDel = SortedList_lookup(list, elem[i].key);
	  SortedList_delete(&elem[i]);
	  __sync_lock_release(&testAndSet);


	}
      else
	{
	  toDel = SortedList_lookup(list, elem[i].key);
	  SortedList_delete(&elem[i]);
	}
    }
  deleted = SortedList_length(list);
  //  fprintf(stdout, "deleted length: %d\n", deleted);

}


void signal_callback_handler(int signum)
{

  fprintf(stderr,"Caught SIGSEGV %d\n", signum);
  exit(1);
}


int main(int argc, char *argv[])
{
    int optParse = 0;
    int threadFlag = 0;
    int iterFlag = 0;
    int yieldFlag = 0;
    int syncFlag = 0;
    int insFlag = 0;
    int delFlag = 0;
    int lookFlag = 0;
    char* threadopt = NULL;
    char* iteropt = NULL;
    char* yieldopt = NULL;
    char* syncoptS = NULL;

    struct timespec start, end;

    int i;
    int rc;
    pthread_attr_t attr;
    void *status;
    srand(time(NULL));    
    
    
    strcpy(tag,"list-");
    static struct option long_options[] = {
      {"threads", required_argument, 0, 't'},
      {"iterations", required_argument, 0, 'i'},
      {"yield", required_argument, 0, 'y'},
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
	  yieldopt = optarg;
	  break;
	case 's':
	  syncFlag = 1;
	  syncopt = optarg[0];
	  syncoptS = optarg;
	  break;
        default:
          fprintf(stderr, "Proper usage of options: --threads=#threads, --iterations=#iterations, --yield=location, --sync=test\n");
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
	strcat(tag, yieldopt); 
	strcat(tag,"-");
	opt_yield = 1;
	fprintf(stderr, "opt_yield: %i", opt_yield);
	for (i = 0; i != '\0'; i++)
	  {
	    if (yieldopt[i] == 'i')
	      opt_yield |= INSERT_YIELD;
	    if (yieldopt[i] == 'd')
	      opt_yield |= DELETE_YIELD;
	    if (yieldopt[i] == 'l')
	      opt_yield |= LOOKUP_YIELD;
	  }
      }
    else 
      {
	strcat(tag,"none-");
      }
    if (syncFlag)
      {
	strcat(tag,syncoptS);
      }
    else 
      {
	strcat(tag,"none");
      }

    //INITIALIZE EMPTY LIST
    //init list

    initList();

    //init elements
    reqNum = threadNum * iterNum;
    elem = malloc(reqNum * sizeof(SortedListElement_t));
    //    fprintf(stdout, "%i\n", reqNum);
    offsetArr = malloc(threadNum *sizeof(int));
    

    initLocks();

    //add keys
    for (i = 0; i < reqNum; i++)
      {
	randSize = rand() % 10;
	randKey = randstring(randSize);
	elem[i].key = randKey;
      }

    if(clock_gettime(CLOCK_MONOTONIC, &start) < 0)sysFailed("clock_gettime",1);
    pthread_t *threads = malloc(threadNum * sizeof(pthread_t));
    int* tids = malloc(threadNum * sizeof(int));

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (i = 0; i < threadNum; i++)
      {
	signal(SIGSEGV, signal_callback_handler);
	tids[i] = i;
	long long offset = i * iterNum;
	offsetArr[i] = i * iterNum;
	//	fprintf(stderr, "before adding offset: %d\n", offsetArr[i]);
	rc = pthread_create(&threads[i], &attr, listAdd, &offsetArr[i]); 

	if (rc) {
	  fprintf(stderr, "ERROR: return code from pthread_create():%d\n", rc);
	  exit(2);
	}
	
      }

    for (i = 0; i < threadNum; i++)
      {
	rc = pthread_join(threads[i], &status);
	if (rc) {
	  fprintf(stderr, "ERROR: return code from pthread_join() is %d\n", rc);
	  exit(2);
	}
      }
    if (clock_gettime(CLOCK_MONOTONIC, &end) < 0)sysFailed("clock_gettime",1);
    ns = end.tv_sec - start.tv_sec;
    ns *=1000000000;
    ns += end.tv_nsec;
    ns -= start.tv_nsec;

    

    int isZero = !SortedList_length(list);
    

    
    long long numOp = threadNum * iterNum * 3;
    //    long long runTime = end.tv_nsec - start.tv_nsec;
    long long aveTime = ns/numOp;
    long long aveMutex = mutex_time/numOp;
    fprintf(stdout, "%s,%i,%i,1,%lli,%lli,%lli,%lli,%lli\n", tag,threadNum, iterNum, numOp, ns, aveTime, aveMutex, mutex_time);

}
