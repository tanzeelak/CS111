#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include "SortedList.h"

#define print_err() do {if (errno) {fprintf(stderr, "error %s", strerr(errno)); exit(1);}} while(0)

pthread_mutex_t count_mutex;
long long count;
int threadNum = 1;
int iterNum = 1;
int opt_yield = 0;
int testAndSet = 0;
char syncopt = NULL;
SortedList_t* list; 
SortedListElement_t* elem;
int randSize = 0;
char* randKey = NULL;
int reqNum;
int spin;


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
  perror("hi");
  if (!randomString) {
    return (char*)0;
  }
  perror("ho");
  unsigned int key = 0;

  for (int n = 0;n < length;n++) {            
    key = rand() % stringLen;          
    randomString[n] = string[key];
  }
  perror("nah");
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

void* listAdd(void* ptr)
{
  int i;
  SortedListElement_t *toDel;
  for (i = 0; i < reqNum; i++)
    {
      randSize = rand() % 10;
      randKey = randstring(randSize);
      elem[i].key = randKey;
      SortedList_insert(list, &elem[i]);
    }
  for (i = 0; i < reqNum; i++)
    {
      toDel = SortedList_lookup(list, elem[i].key);
      SortedList_delete(&elem[i]);
    }
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
    
    struct timespec start, end;

    int i;
    int rc;
    pthread_attr_t attr;
    void *status;
    srand(time(NULL));    

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
	for (i = 0; i != '\0'; i++)
	  {
	    if (yieldopt[i] == 'i')
	      insFlag = 1;
	    if (yieldopt[i] == 'd')
	      delFlag = 1;
	    if (yieldopt[i] == 'l')
	      lookFlag = 1;
	  }
      }



    
    //INITIALIZE EMPTY LIST
    //init list

    initList();

    //init elements
    reqNum = threadNum * iterNum;
    elem = malloc(reqNum * sizeof(SortedListElement_t));
    fprintf(stdout, "%i\n", reqNum);

    initLocks();


    //add node
    for (i = 0; i < reqNum; i++)
      {
	randSize = rand() % 10;
	fprintf(stdout, "%i\n", randSize);
	randKey = randstring(randSize);
	fprintf(stdout, "before insertion randkey: %s\n", randKey);
	elem[i].key = randKey;
	SortedList_insert(list, &elem[i]);
      }

    int sizeO = SortedList_length(list);
    fprintf(stdout, "list size: %i\n", sizeO);



    
    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_t *threads = malloc(threadNum * sizeof(pthread_t));
    int* tids = malloc(threadNum * sizeof(int));

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (i = 0; i < threadNum; i++)
      {
	//use list insert elements instead of threadAdd and count
	//in func, gets list slenght
	//looks upa nd dletes prev inserted keeys
	tids[i] = i;
	rc = pthread_create(&threads[i], &attr, listAdd, &count); 
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

    /*
    
    long long numOp = threadNum * iterNum * 2;
    long long runTime = end.tv_nsec - start.tv_nsec;
    long long aveTime = runTime/numOp;
    fprintf(stdout, "add-none,%i,%i,%i,%lli,%lli,%lli\n", threadNum, iterNum, numOp, runTime, aveTime, count);



*/
}
