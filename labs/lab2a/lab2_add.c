#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define print_err() do {if (errno) {fprintf(stderr, "error %s", strerr(errno)); exit(1);}} while(0)


struct Data
{
  int long long *ptr;
  int long long iterNum;
};

pthread_mutex_t mutex;
int threadNum = 1;
int iterNum = 1;
//struct Data dat;

void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  *pointer = sum;
}


void fakeAdd(struct Data *datPass)
{
  int i;
  for (i = 0; i < datPass->iterNum; i++)
    ++(*(datPass->ptr));
  for (i = 0; i < datPass->iterNum; i++)
    --(*(datPass->ptr));
  
}

void threadAdd(int num)
{
  //  void add(long long *pointer, long long value) 
  //y{
  //    long long sum = *pointer + value;
  //    *pointer = sum;
    //  }

}

int main(int argc, char *argv[])
{

    int optParse = 0;
    int threadFlag = 0;
    int iterFlag = 0;
    char* threadopt = NULL;
    char* iteropt = NULL;
    struct timespec start, end;
    //    int long long counter = 0;
    int i;
    int rc;
    pthread_attr_t attr;
    void *status;

    static struct option long_options[] = {
      {"threads", required_argument, 0, 't'},
      {"iterations", required_argument, 0, 'i'},
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

    struct Data dat;

    dat.ptr = malloc(sizeof(long long));
    *(dat.ptr) = 0;

    dat.iterNum = iterNum;



    clock_gettime(CLOCK_MONOTONIC, &start);


    pthread_t *threads = malloc(threadNum * sizeof(pthread_t));

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (i = 0; i < threadNum; i++)
      {
	rc = pthread_create(&threads[i], &attr, fakeAdd, &dat);
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
    printf("sum: %lli\n",*(dat.ptr));

}
