#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>
#include <mraa/aio.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#define print_err() do {if (errno) {fprintf(stderr, "error %s", strerr(errno)); exit(1);}} while(0)

float tempC, tempF;
int tempType = 0;
int stopFlag = 0;
int shutdownFlag = 0;
int logFlag = 0;
FILE* lfd;
int perFlag=0;

time_t timer;
char timeBuff[9];
struct tm* timeInfo;
mraa_aio_context tempSensor;
mraa_gpio_context button;
const int B = 4275;
int rawTemp;

void sysFailed(char* sysCall, int exitNum)
{
  fprintf(stderr, "%s failed: %s\n", sysCall, strerror(errno));
  exit(exitNum);
}


void signal_callback_handler(int signum)
{
  fprintf(stderr,"Caught SIGSEGV %d\n", signum);
  exit(1);
}

void* print() 
{
	tempSensor = mraa_aio_init(0);
	while(1)
	{
		rawTemp = mraa_aio_read(tempSensor);
		double R = 1023.0/((double)rawTemp) - 1.0;
		R = 10000.0*R;
		tempC  = 1.0/(log(R/100000.0)/B + 1/298.15) - 273.15;
		tempF = tempC * 9/5 + 32;

		time(&timer);
		timeInfo = localtime(&timer);
		strftime(timeBuff, 9, "%H:%M:%S", timeInfo);
	}
}


int main(int argc, char *argv[])
{
	int optParse = 0;
	int perNum = 0; 
	int scaleFlag = 0;
	char* peropt = NULL;
	char scaleopt = 'F';
	char* logopt = NULL;
	char* outopt = NULL;
    	int i;
    
    
    static struct option long_options[] = {
      {"period", required_argument, 0, 'p'},
      {"scale", required_argument, 0, 's'},
      {"log", required_argument, 0, 'l'},
      {0,0,0,0}
    };

    int option_index = 0;
    while((optParse = getopt_long(argc, argv, "i:o:sc:", long_options, &option_index)) != -1){
      switch (optParse)
        {
        case 'p':
          perFlag = 1;
          peropt = optarg;
          break;
        case 's':
          scaleFlag = 1;
          scaleopt = optarg[0];
          break;
	case 'l':
	  logFlag = 1;
	  logopt = optarg;
	  break;
        default:
          fprintf(stderr, "Proper usage of options: --threads=#threads, --iterations=#iterations, --yield=location, --sync=test --list=#lists\n");
          exit(1);
        }
    }

    if (perFlag)
	perNum = atoi(peropt);
    if (scaleFlag)
    {
    }
    if (logFlag)
    {
	    int ofd = creat(outopt, 0666);
	    if (ofd >= 0) {
	    	close(1);
		dup(ofd);
		close(ofd);
 	    } else {
		fprintf(stderr, "Failed to create output file. %s\n", strerror(errno));
		exit(2);
	    }
    }
    
    exit(0);
}
