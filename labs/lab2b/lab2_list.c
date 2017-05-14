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
int listNum = 1;
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
   list = malloc(sizeof(SortedList_t)*listNum);
   for (int i = 0; i < listNum; i++){
     list[i].key = NULL;
     list[i].next = &list[i];
     list[i].prev = &list[i];
   }
}

void initLocks(void)
{
  if (syncopt == 'm')
    pthread_mutex_init(&count_mutex,NULL);
  if (syncopt == 's')
      spin = 0; 
}

unsigned int hash(const char* key)
{
  unsigned long counter;
  unsigned long hashAddress = 0;
  for (counter = 0; key[counter] != '\0'; counter++)
    {
      hashAddress = key[counter] + (hashAddress << 6) + (hashAddress << 16) - hashAddress;
    }
  return hashAddress;
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

void* listAdd(void* offset)
{
  int i, added, deleted;
  SortedListElement_t *toDel;

  for (i = *(int*)offset; i < *(int*)offset+iterNum; i++)
    {
      unsigned int listId = hash(elem[i].key) % listNum;
      if (syncopt == 'm')
	{
	  clock_gettime(CLOCK_MONOTONIC, &start_lock);
	  pthread_mutex_lock(&count_mutex);
	  clock_gettime(CLOCK_MONOTONIC, &end_lock);
	  mutex_time += 1000000000L * (end_lock.tv_sec - start_lock.tv_sec) + end_lock.tv_nsec - start_lock.tv_nsec;

	  SortedList_insert(&list[listId], &elem[i]);
	  pthread_mutex_unlock(&count_mutex);

	}
      else if (syncopt == 's')
	{

	  clock_gettime(CLOCK_MONOTONIC, &start_lock);

  	  while(__sync_lock_test_and_set(&testAndSet, 1));

	  clock_gettime(CLOCK_MONOTONIC, &end_lock);
	  mutex_time += 1000000000L * (end_lock.tv_sec - start_lock.tv_sec) + end_lock.tv_nsec - start_lock.tv_nsec;

	  SortedList_insert(&list[listId], &elem[i]);
	  __sync_lock_release(&testAndSet);
	}
      else
	{
	  SortedList_insert(&list[listId], &elem[i]);
	}
    }
  if(SortedList_length(list) == -1)
    {
      fprintf(stderr, "List is corrupted\n");
      exit(2);
    }

  for (i = *(int*)offset; i < *(int*)offset+iterNum; i++)
    {

      unsigned int listId = hash(elem[i].key) % listNum;
      if (syncopt == 'm')
	{
	  clock_gettime(CLOCK_MONOTONIC, &start_lock);

	  pthread_mutex_lock(&count_mutex);
	  clock_gettime(CLOCK_MONOTONIC, &end_lock);
	  mutex_time += 1000000000L * (end_lock.tv_sec - start_lock.tv_sec) + end_lock.tv_nsec - start_lock.tv_nsec;

	  if(SortedList_lookup(&list[listId], elem[i].key) == NULL)
	    {
	      fprintf(stderr,"Element not found in lookup.\n");
	      exit(2);
	    }

	  SortedList_delete(&elem[i]);
	  pthread_mutex_unlock(&count_mutex);

	}
      else if (syncopt == 's')
	{
	  clock_gettime(CLOCK_MONOTONIC, &start_lock);

  	  while(__sync_lock_test_and_set(&testAndSet, 1));
	  clock_gettime(CLOCK_MONOTONIC, &end_lock);
	  mutex_time += 1000000000L * (end_lock.tv_sec - start_lock.tv_sec) + end_lock.tv_nsec - start_lock.tv_nsec;

	  if(SortedList_lookup(&list[listId], elem[i].key) == NULL)
	    {
	      fprintf(stderr,"Element not found in lookup.\n");
	      exit(2);
	    }
	  SortedList_delete(&elem[i]);
	  __sync_lock_release(&testAndSet);


	}
      else
	{
	  if(SortedList_lookup(&list[listId], elem[i].key) == NULL)
	    {
	      fprintf(stderr,"Element not found in lookup.\n");
	      exit(2);
	    }
	  SortedList_delete(&elem[i]);
	}
    }
  if (SortedList_length(list) == -1)
    {
      fprintf(stderr,"List is corrupted.\n");
      exit(2);
    }
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
    int listFlag = 0; 
    char* threadopt = NULL;
    char* iteropt = NULL;
    char* yieldopt = NULL;
    char* syncoptS = NULL;
    char* listopt = NULL;

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
      {"lists", required_argument, 0, 'l'},
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
	case 'l':
	  listFlag = 1;
	  listopt = optarg;
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
    if (listFlag)
      {
	listNum = atoi(listopt);
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
    long long aveTime = ns/numOp;
    long long aveMutex = mutex_time/numOp;
    fprintf(stdout, "%s,%i,%i,%i,%lli,%lli,%lli,%lli\n", tag,threadNum, iterNum, listNum, numOp, ns, aveTime, aveMutex);

}
